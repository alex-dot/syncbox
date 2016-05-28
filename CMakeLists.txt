cmake_minimum_required(VERSION 2.8)
project("syncbox")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

# C++11 support
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# print all Warnings
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
# enable debugging
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -g -O0 -ggdb")

# Threads
find_package(Threads)

# Boost libraries
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 
find_package(Boost 1.55.0 COMPONENTS system filesystem thread unit_test_framework program_options) 

# - Find Crypto++
# courtesy of: 
# https://github.com/harningt/cryptoface/blob/master/Modules/FindCrypto%2B%2B.cmake
if(CRYPTO++_INCLUDE_DIR AND CRYPTO++_LIBRARIES)
   set(CRYPTO++_FOUND TRUE)

else(CRYPTO++_INCLUDE_DIR AND CRYPTO++_LIBRARIES)
  find_path(CRYPTO++_INCLUDE_DIR cryptlib.h
      /usr/include/crypto++
      /usr/include/cryptopp
      /usr/local/include/crypto++
      /usr/local/include/cryptopp
      /opt/local/include/crypto++
      /opt/local/include/cryptopp
      $ENV{SystemDrive}/Crypto++/include
      )

  find_library(CRYPTO++_LIBRARIES NAMES cryptopp
      PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      $ENV{SystemDrive}/Crypto++/lib
      )

  if(CRYPTO++_INCLUDE_DIR AND CRYPTO++_LIBRARIES)
    set(CRYPTO++_FOUND TRUE)
    message(STATUS "Found Crypto++: ${CRYPTO++_INCLUDE_DIR}, ${CRYPTO++_LIBRARIES}")
  else(CRYPTO++_INCLUDE_DIR AND CRYPTO++_LIBRARIES)
    set(CRYPTO++_FOUND FALSE)
    message(STATUS "Crypto++ not found.")
  endif(CRYPTO++_INCLUDE_DIR AND CRYPTO++_LIBRARIES)

  mark_as_advanced(CRYPTO++_INCLUDE_DIR CRYPTO++_LIBRARIES)

endif(CRYPTO++_INCLUDE_DIR AND CRYPTO++_LIBRARIES)

# - Find ZMQ
# courtesy of: 
# http://pastebin.com/fLsm9SYf
FIND_PATH ( ZEROMQ_INCLUDE_DIR NAMES zmq.h PATHS /usr/include/ /usr/local/include/ )
FIND_LIBRARY ( ZEROMQ_LIBRARY NAMES zmq PATHS /usr/lib /usr/local/lib )
IF ( ZEROMQ_INCLUDE_DIR AND ZEROMQ_LIBRARY )
    SET ( ZEROMQ_FOUND TRUE )
ENDIF ( ZEROMQ_INCLUDE_DIR AND ZEROMQ_LIBRARY )

IF ( ZEROMQ_FOUND )
  IF ( NOT ZEROMQ_FIND_QUIETLY )
    MESSAGE ( STATUS "Found ZeroMQ:" )
    MESSAGE ( STATUS "  (Headers)       ${ZEROMQ_INCLUDE_DIR}" )
    MESSAGE ( STATUS "  (Library)       ${ZEROMQ_LIBRARY}" )
  ENDIF ( NOT ZEROMQ_FIND_QUIETLY )
ELSE ( ZEROMQ_FOUND )
  MESSAGE ( FATAL_ERROR "Could not find ZeroMQ" )
ENDIF ( ZEROMQ_FOUND )

# - Find msgpack
if(MSGPACK_INCLUDE_DIR AND MSGPACK_LIBRARIES)
  set(MSGPACK_FOUND TRUE)

else(MSGPACK_INCLUDE_DIR AND MSGPACK_LIBRARIES)
  find_path ( MSGPACK_INCLUDE_DIR NAMES msgpack.hpp PATHS /usr/include/ /usr/local/include/ )
  find_library ( MSGPACK_LIBRARY NAMES msgpack PATHS /usr/lib /usr/local/lib )
  if ( MSGPACK_INCLUDE_DIR AND MSGPACK_LIBRARY )
    set ( MSGPACK_FOUND TRUE )
  endif ( MSGPACK_INCLUDE_DIR AND MSGPACK_LIBRARY )

  if ( MSGPACK_FOUND )
    if ( NOT MSGPACK_FIND_QUIETLY )
      message ( STATUS "Found MessagePack:" )
      message ( STATUS "  (Headers)       ${MSGPACK_INCLUDE_DIR}" )
      message ( STATUS "  (Library)       ${MSGPACK_LIBRARY}" )
    endif ( NOT MSGPACK_FIND_QUIETLY )
  else ( MSGPACK_FOUND )
    message ( FATAL_ERROR "Could not find MessagePack" )
  endif ( MSGPACK_FOUND )

endif(MSGPACK_INCLUDE_DIR AND MSGPACK_LIBRARIES)

# enable testing
enable_testing()
subdirs(test)

# go to source
subdirs(src)