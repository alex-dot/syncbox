/*
 * Config
 */

#include "constants.hpp"
#include "config.hpp"

#include <zmq.hpp>
#include <string>
#include <sstream>
#include <iostream>


Config::~Config()
{
}

Config* Config::initialize(zmq::context_t* z_ctx_)
{
}
