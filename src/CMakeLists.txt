include_directories(../include)
add_executable(flocksy  main.cpp
                        constants.cpp
                        config.cpp
                        transmitter.cpp
                        directory.cpp
                        box.cpp
                        hash_tree.cpp
                        hash.cpp
                        file.cpp
                        boxoffice.cpp
                        publisher.cpp
                        heartbeater.cpp
                        dispatcher.cpp
                        subscriber.cpp)
target_link_libraries(flocksy ${CMAKE_THREAD_LIBS_INIT}
                              ${Boost_LIBRARIES}
                              ${LIBSODIUM_LIBRARY}
                              ${ZEROMQ_LIBRARY}
                              ${ZEROMQ_CPP_LIBRARY}
                              ${JSONCPP_LIBRARY})