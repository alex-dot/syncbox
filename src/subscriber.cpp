/*
 * Subscriber
 */

#include "constants.hpp"
#include "subscriber.hpp"

#include <unistd.h>

#include <zmqpp/zmqpp.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <boost/thread.hpp>

Subscriber::Subscriber(zmqpp::context* z_ctx_, node_t data_) :
  Transmitter(z_ctx_), 
  z_subscriber(nullptr),
  data(data_) {
    tac = (char*)"sub";
}

Subscriber::~Subscriber()
{
  z_subscriber->close();
  delete z_subscriber;
}

int Subscriber::run()
{
  // internal check if subscriber was correctly initialized
  if ( z_ctx == nullptr || data.endpoint.compare("") == 0
      || ( data.sb_subtype != SB_SUBTYPE_TCP_BIDIR && data.sb_subtype != SB_SUBTYPE_TCP_UNIDIR ) )
    return 1;

  if (SB_MSG_DEBUG) printf("sub: receiving from publisher...\n");
  Config* conf = Config::getInstance();
  zmqpp::curve::keypair host_keypair = conf->getHostKeypair();
  z_subscriber = new zmqpp::socket(*z_ctx, zmqpp::socket_type::sub);
  z_subscriber->set(zmqpp::socket_option::curve_server_key, data.public_key);
  z_subscriber->set(zmqpp::socket_option::curve_public_key, host_keypair.public_key);
  z_subscriber->set(zmqpp::socket_option::curve_secret_key, host_keypair.secret_key);
  z_subscriber->connect(data.endpoint.c_str());
  z_subscriber->subscribe("");

  std::stringstream* sstream;
  int msg_type, msg_signal;
  std::string infomessage;

  while (true)
  {
    sstream = new std::stringstream();
    s_recv(*z_boxoffice_push, *z_broadcast, *z_subscriber, *sstream);
    *sstream >> msg_type;
    if ( msg_type == SB_SIGTYPE_LIFE ) {
      *sstream >> msg_signal;
      if ( msg_signal == SB_SIGLIFE_INTERRUPT ) break;
    } else {
      msg_signal = msg_type;
    }

    std::string infomessage = sstream->str().substr(3);
    std::stringstream message;
    message << SB_SIGTYPE_SUB << " "
            << msg_signal;
    message.write(reinterpret_cast<char*>(
                    const_cast<unsigned char*>(data.uid)
                  ), SB_GENERIC_HASH_LEN);
    message << infomessage;
    zmqpp::message z_msg;
    z_msg << message.str();
    z_boxoffice_pull->send(z_msg);

    delete sstream;
  }
  delete sstream;

  return 0;
}