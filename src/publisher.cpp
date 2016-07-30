/*
 * Publisher
 */

#include "constants.hpp"
#include "publisher.hpp"

#include <unistd.h>
#include <endian.h>

#include <zmqpp/zmqpp.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include <boost/thread.hpp>

Publisher::Publisher(zmqpp::context* z_ctx_, host_t data_) :
  Transmitter(z_ctx_),
  z_heartbeater(nullptr),
  z_dispatcher(nullptr),
  z_publisher(nullptr),
  data(data_) {
    tac = (char*)"pub";
    this->connectToHeartbeater();
    this->connectToDispatcher();
}

Publisher::~Publisher() {
  z_heartbeater->close();
  z_dispatcher->close();
  z_publisher->close();

  delete z_heartbeater;
  delete z_dispatcher;
  delete z_publisher;
}

int Publisher::connectToHeartbeater()
{
  z_heartbeater = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pair);
  z_heartbeater->connect("inproc://pub_hb_pair");

  return 0;
}

int Publisher::connectToDispatcher()
{
  z_dispatcher = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pair);
  z_dispatcher->connect("inproc://pub_disp_pair");

  return 0;
}

int Publisher::run()
{
  // internal check if publisher was correctly initialized
  if ( z_ctx == nullptr || data.endpoint.compare("") == 0 )
    return 1;

  if (SB_MSG_DEBUG) printf("pub: setting up authentication...\n");
  zmqpp::auth authenticator(*z_ctx);
  if (SB_MSG_DEBUG) authenticator.set_verbose (true);
  authenticator.configure_domain("*");

  Config* conf = Config::getInstance();
  std::vector<std::string> node_endpoints = conf->getNodePublicKeys();
  for (std::vector<std::string>::iterator i=node_endpoints.begin();
       i != node_endpoints.end(); ++i) {
    authenticator.configure_curve(*i);
  }

  if (SB_MSG_DEBUG) printf("pub: starting pub socket and sending...\n");
  z_publisher = new zmqpp::socket(*z_ctx, zmqpp::socket_type::pub);
  z_publisher->set(zmqpp::socket_option::identity, data.uid);
  z_publisher->set(zmqpp::socket_option::curve_server, 1);
  z_publisher->set(zmqpp::socket_option::curve_secret_key, data.keypair.secret_key);
  z_publisher->bind(data.endpoint.c_str());

  std::stringstream* sstream;
  int msg_type, msg_signal;

  while(true)
  {
    // waiting for boxoffice input in non-blocking mode
    sstream = new std::stringstream();
    s_recv(*z_boxoffice_push,
           *z_broadcast,
           *z_heartbeater,
           *z_dispatcher,
           *sstream);

    *sstream >> msg_type >> msg_signal;
    if ( msg_type == SB_SIGTYPE_LIFE && msg_signal == SB_SIGLIFE_INTERRUPT ) break;

    // send a message
    std::string infomessage = sstream->str().substr(5);
    std::stringstream message;
    message << std::to_string(msg_signal) << infomessage;
    if (msg_signal != fsm::status_200 && msg_signal != fsm::status_210 )
      if (SB_MSG_DEBUG) printf("pub: sending status %d message with length %lu\n",
          msg_signal, message.str().length());
    // setting the width to SB_MINIMUM_HB_WIDTH so all heartbeats have the same length
    int p = message.tellp();
    if (SB_MINIMUM_HB_WIDTH-p > 0)
      message << std::setw(SB_MINIMUM_HB_WIDTH-p) << std::setfill(' ') << " ";
    zmqpp::message z_msg;
    z_msg << message.str();
    z_publisher->send(z_msg);

    delete sstream;
  }

  delete sstream;

  return 0;
}