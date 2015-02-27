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

  private:
    Boxoffice() {};
    ~Boxoffice() {};

    int connectToPubsAndSubs(zmq::context_t* z_ctx);
};

#endif