include_directories(../include)

add_executable(syncbox_pub  main_pub.cpp
                            directory.cpp
                            box.cpp
                            hash_tree.cpp
                            hash.cpp)
target_link_libraries(syncbox_pub ${Boost_LIBRARIES}
                                  ${CRYPTO++_LIBRARIES}
                                  ${ZEROMQ_LIBRARY})

add_executable(syncbox_sub  main_sub.cpp
                            directory.cpp
                            box.cpp
                            hash_tree.cpp
                            hash.cpp)
target_link_libraries(syncbox_sub ${Boost_LIBRARIES}
                                  ${CRYPTO++_LIBRARIES}
                                  ${ZEROMQ_LIBRARY})
