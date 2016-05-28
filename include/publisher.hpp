/**
 * Each configured access point shall have its own publisher to send 
 * its configured box data across. Every publisher binds sockets where it 
 * publishes box state changes for an arbitrary number of (external) 
 * subscribers to receive. It shall react on messages from the boxoffice 
 * and send the appropriate messages accordingly. 
 * For each publisher there shall be a separate publisher thread. 
 */

#ifndef SB_PUBLISHER_HPP
#define SB_PUBLISHER_HPP

#include <zmq.hpp>

class Publisher
{
  public:
    Publisher() :
      z_ctx(nullptr),
      z_boxoffice_pull(nullptr),
      z_boxoffice_push(nullptr),
      z_publisher(nullptr),
      z_broadcast(nullptr),
      endpoint("")
      {};
    Publisher(zmq::context_t* z_ctx_, std::string endpoint_) :
      z_ctx(z_ctx_),
      z_boxoffice_pull(nullptr),
      z_boxoffice_push(nullptr),
      z_publisher(nullptr),
      z_broadcast(nullptr),
      endpoint(endpoint_)
      {};
    Publisher(const Publisher&);
    ~Publisher();

    static Publisher* initialize(zmq::context_t* z_ctx_, 
                                 std::string endpoint_);
    int setContext(zmq::context_t* z_ctx_)
    {
      z_ctx = z_ctx_;
      if ( z_ctx == nullptr )
        return 0;
      else
        return 1;

      return -1;
    }
    int sendExitSignal();
    int run();

  private:
    int connectToBroadcast();
    int connectToBoxoffice();

    zmq::context_t* z_ctx;
    zmq::socket_t* z_boxoffice_pull;
    zmq::socket_t* z_boxoffice_push;
    zmq::socket_t* z_publisher;
    zmq::socket_t* z_broadcast;
    std::string endpoint;
};

#endif