/*
 * The publisher is a singleton that binds sockets where it publishes box 
 * state changes for an arbitrary number of (external) subscribers to 
 * receive. It shall react on messages from the boxoffice and send the 
 * appropriate messages accordingly. 
 * Publisher shall only be used within the publisher thread. 
 */

#ifndef SB_PUBLISHER_HPP
#define SB_PUBLISHER_HPP

#include <zmq.hpp>

class Publisher
{
  public:
    Publisher(const Publisher&) = delete;
    Publisher& operator=(const Publisher&) = delete;

    static Publisher* getInstance()
    {
      static Publisher pub_instance_;
      return &pub_instance_;
    }

    static Publisher* initialize(zmq::context_t* z_ctx_);
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
    Publisher() :
      z_ctx(nullptr),
      z_pub_pair(nullptr),
      channel_list()
      {};
    ~Publisher() {};

    int connectToBoxoffice();
    int startPubChannel(std::string channel_name);
    int stopPubChannel(zmq::socket_t* channel);
    int listenOnChannels();

    zmq::context_t* z_ctx;
    zmq::socket_t* z_pub_pair;
    std::vector<zmq::socket_t*> channel_list;
};

#endif