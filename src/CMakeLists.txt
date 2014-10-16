include_directories(../include)
add_executable(syncbox  main.cpp
                        directory.cpp
                        box.cpp
                        database.cpp
                        hash_tree.cpp
                        hash.cpp)
target_link_libraries(syncbox ${Boost_LIBRARIES}
                              ${CRYPTO++_LIBRARIES})
