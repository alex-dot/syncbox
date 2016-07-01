include_directories(../include)
add_executable(syncbox  main.cpp
                        constants.cpp
                        config.cpp
                        transmitter.cpp
                        directory.cpp
                        box.cpp
                        hash_tree.cpp
                        hash.cpp
                        boxoffice.cpp
                        publisher.cpp
                        heartbeater.cpp
                        subscriber.cpp)
target_link_libraries(syncbox ${CMAKE_THREAD_LIBS_INIT}
                              ${Boost_LIBRARIES}
                              ${CRYPTO++_LIBRARIES}
                              ${ZEROMQ_LIBRARY}
                              ${ZEROMQ_CPP_LIBRARY}
                              ${JSONCPP_LIBRARY})