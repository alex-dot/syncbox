
add_test(NAME hash_structors COMMAND syncboxTest -t hash_structors)
add_test(NAME hash_makeHash COMMAND syncboxTest -t hash_makeHash)
add_test(NAME hash_tree_size_compare COMMAND syncboxTest -t hash_tree_size_compare)
# add_test(NAME hash_tree_size_compare_random COMMAND syncboxTest -t hash_tree_size_compare_random)
# add_test(NAME hash_tree_compare COMMAND syncboxTest -t hash_tree_compare)
add_test(NAME directory_manual COMMAND syncboxTest -t directory_manual)
add_test(NAME directory_auto COMMAND syncboxTest -t directory_auto)
add_test(NAME box_test COMMAND syncboxTest -t box_test)


include_directories(../include)
add_executable(syncboxTest testMain.cpp
                           ../src/hash.cpp
                           ../src/hash_tree.cpp
                           ../src/directory.cpp
                           ../src/box.cpp)
target_link_libraries(syncboxTest ${Boost_LIBRARIES}
                                  ${CRYPTO++_LIBRARIES})