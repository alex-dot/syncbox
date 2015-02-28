/*
 * generic constants for application
 */

#ifndef SB_CONSTANTS_HPP
#define SB_CONSTANTS_HPP

enum SB_SIGTYPE {
  SB_SIGTYPE_LIFE,
  SB_SIGTYPE_PUB
};
enum SB_SIGLIFE {
  SB_SIGLIFE_EXIT,
  SB_SIGLIFE_ALIVE,
  SB_SIGLIFE_ERROR=-1
};
enum SB_SIGPUB {
  SB_SIGPUB_GET_CHANNELS
};

#endif