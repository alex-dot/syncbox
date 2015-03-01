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
    int setContext(zmq::context_t* z_ctx_)
    {
      z_ctx = z_ctx_;
      if ( z_ctx == nullptr )
        return 0;
      else
        return 1;

      return -1;
    }

  private:
    Boxoffice() :
      pub_thread(nullptr),
      z_ctx(nullptr),
      z_bo_main(nullptr),
      z_pub_pair(nullptr),
      z_router(nullptr)
      {};
    ~Boxoffice() {};

    int connectToMain();
    int setupPublisher();
    int connectToPub();
    int setupSubscribers();
    int runRouter();
    int closeConnections();

    boost::thread* pub_thread;
    zmq::context_t* z_ctx;
    zmq::socket_t* z_bo_main;
    zmq::socket_t* z_pub_pair;
    zmq::socket_t* z_router;
    //std::vector< std::pair<std::string,int> > subscribers; // endpoint and type
    std::vector<boost::thread*> sub_threads;

  public: // TODO: settings-serialization -> make private again
    std::vector< std::pair<std::string,int> > subscribers; // endpoint and type
};

#endif