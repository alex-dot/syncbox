/*
 * The boxoffice is a singleton that acts as a broker between the files 
 * contained in boxes and the subscribers/publishers. It uses the shared queue
 * pattern of ZeroMQ so that subscribers forward their received messages
 * to the boxoffice and the boxoffice prepares the messages for the 
 * publishers to send. 
 * Boxoffice shall only be used within the boxoffice thread. 
 *
 * \TODO heartbeaters+dispatchers probably need info to check if the call
 *       was meant for them. right now all hb+disp would react
 * \TODO boxoffice must be able to handle multiple transmissions of
 *       different boxes
 * \TODO current_box needn't be transmitted, only once. Should be stored
 *       in member variable
 */

#ifndef SB_BOXOFFICE_HPP
#define SB_BOXOFFICE_HPP

#include <zmqpp/zmqpp.hpp>
#include <boost/thread.hpp>
#include <vector>
#include <deque>
#include <utility>
#include <unordered_map>

#include "file.hpp"
#include "box.hpp"
#include "config.hpp"

namespace fsm {
  #include "tarmuft_fsm.h"
}

class Boxoffice
{
  public:
    Boxoffice(const Boxoffice&) = delete;
    Boxoffice& operator=(const Boxoffice&) = delete;

    static Boxoffice* getInstance()
    {
      static Boxoffice bo_instance_;
      return &bo_instance_;
    }

    static Boxoffice* initialize(zmqpp::context* z_ctx);
    int setContext(zmqpp::context* z_ctx_);

  private:
    Boxoffice() :
      state_(fsm::ready_state),
      file_list_metadata_(),
      file_list_data_(),
      node_reply_counter_(0),
      total_node_number_(0),
      current_timing_offset_(-1),
      notified_dispatch_(false),
      current_box_(new unsigned char[SB_GENERIC_HASH_LEN]),
      current_file_(),
      file_metadata_written_(false),
      z_ctx(nullptr),
      z_bo_main(nullptr),
      z_router(nullptr),
      z_bo_pub(nullptr),
      z_bo_hb(nullptr),
      z_bo_disp(nullptr),
      z_broadcast(nullptr),
      subscribers(),
      publishers(),
      boxes(),
      pub_threads(),
      hb_threads(),
      sub_threads(),
      box_threads()
      {};
    ~Boxoffice();

    int connectToBroadcast();
    int connectToMain();
    int setupConnectionToChildren();
    int setupBoxes();
    int setupPublishers();
    int setupHeartbeaters();
    int setupDispatchers();
    int setupSubscribers();
    int checkChildren();
    int runRouter();
    int closeConnections();

    int processEvent(fsm::status_t status, std::stringstream* message);
    bool checkEvent(fsm::state_t const state,
                    fsm::event_t const event,
                    fsm::status_t const status) const;
    int performAction(fsm::event_t const event,
                      fsm::action_t const action,
                      fsm::status_t const received_status,
                      fsm::state_t const new_state);
    int updateHeartbeat(fsm::status_t const new_status,
                        fsm::state_t const new_state);
    void prepareHeartbeatMessage(std::stringstream* message,
                                 fsm::state_t const new_state);
    int updateTimestamp(std::stringstream* sstream);

    fsm::state_t state_;

    std::deque< File* > file_list_metadata_;
    std::deque< File* > file_list_data_;
    int node_reply_counter_;
    int total_node_number_;
    uint32_t current_timing_offset_;
    bool notified_dispatch_;
    unsigned char* current_box_;
    std::stringstream current_file_;
    bool file_metadata_written_;

    zmqpp::context* z_ctx;
    zmqpp::socket* z_bo_main;
    zmqpp::socket* z_router;
    zmqpp::socket* z_bo_pub;
    zmqpp::socket* z_bo_hb;
    zmqpp::socket* z_bo_disp;
    zmqpp::socket* z_broadcast;

    node_map              subscribers; // endpoint and type
    std::vector< host_t > publishers;
    box_map               boxes;

    std::vector<boost::thread*> pub_threads;
    std::vector<boost::thread*> hb_threads;
    std::vector<boost::thread*> disp_threads;
    std::vector<boost::thread*> sub_threads;
    std::vector<boost::thread*> box_threads;
};

#endif