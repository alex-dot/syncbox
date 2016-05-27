/*
 * The boxoffice is a singleton that acts as a broker between the files 
 * contained in boxes and the subscribers/publishers. It uses the shared queue
 * pattern of ZeroMQ so that subscribers forward their received messages
 * to the boxoffice and the boxoffice prepares the messages for the 
 * publishers to send. 
 * Boxoffice shall only be used within the boxoffice thread. 
 */

#ifndef SB_BOXOFFICE_HPP
#define SB_BOXOFFICE_HPP

#include <zmq.hpp>
#include <boost/thread.hpp>
#include <vector>
#include <utility>
#include <unordered_map>

#include "box.hpp"

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

    static Boxoffice* initialize(zmq::context_t* z_ctx);
    int setContext(zmq::context_t* z_ctx_);

  private:
    Boxoffice() :
      z_ctx(nullptr),
      z_bo_main(nullptr),
      z_router(nullptr),
      z_broadcast(nullptr),
      pub_threads(),
      sub_threads(),
      box_threads(),
      boxes()
      {};
    ~Boxoffice();

    int connectToBroadcast();
    int connectToMain();
    int setupBoxes();
    int setupPublishers();
    int setupSubscribers();
    int checkChildren();
    int runRouter();
    int closeConnections();

    zmq::context_t* z_ctx;
    zmq::socket_t* z_bo_main;
    zmq::socket_t* z_router;
    zmq::socket_t* z_broadcast;
    //std::vector< std::pair<std::string,int> > subscribers; // endpoint and type
    std::vector< std::string > publishers;
    std::vector<boost::thread*> pub_threads;
    std::vector<boost::thread*> sub_threads;
    std::vector<boost::thread*> box_threads;
    std::unordered_map<int,Box*> boxes;

  public: // TODO: settings-serialization -> make private again
    std::vector< std::pair<std::string,int> > subscribers; // endpoint and type
};

#endif