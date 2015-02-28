include_directories(../include)
add_executable(syncbox  main.cpp
                        directory.cpp
                        box.cpp
                        hash_tree.cpp
                        hash.cpp
                        boxoffice.cpp
                        publisher.cpp)
target_link_libraries(syncbox ${CMAKE_THREAD_LIBS_INIT}
                              ${Boost_LIBRARIES}
                              ${CRYPTO++_LIBRARIES}
                              ${ZEROMQ_LIBRARY})