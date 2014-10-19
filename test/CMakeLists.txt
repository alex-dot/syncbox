add_test(NAME hash_structors COMMAND syncboxTest -t hash_structors)
add_test(NAME hash_makeHash COMMAND syncboxTest -t hash_makeHash)
add_test(NAME hash_compare COMMAND syncboxTest -t hash_compare)
add_test(NAME hash_tree_size_compare COMMAND syncboxTest -t hash_tree_size_compare)
# add_test(NAME hash_tree_size_compare_random COMMAND syncboxTest -t hash_tree_size_compare_random)
# add_test(NAME hash_tree_compare COMMAND syncboxTest -t hash_tree_compare)
add_test(NAME hash_tree_compare_diff COMMAND syncboxTest -t hash_tree_compare_diff)
add_test(NAME hash_tree_sort COMMAND syncboxTest -t hash_tree_sort)
add_test(NAME hash_tree_top_hash COMMAND syncboxTest -t hash_tree_top_hash)
add_test(NAME directory_manual COMMAND syncboxTest -t directory_manual)
add_test(NAME directory_auto COMMAND syncboxTest -t directory_auto)
add_test(NAME directory_compare COMMAND syncboxTest -t directory_compare)
# add_test(NAME box_test COMMAND syncboxTest -t box_test)
add_test(NAME box_compare COMMAND syncboxTest -t box_compare)


include_directories(../include)
add_executable(syncboxTest testMain.cpp
                           ../src/hash.cpp
                           ../src/hash_tree.cpp
                           ../src/directory.cpp
                           ../src/box.cpp)
target_link_libraries(syncboxTest ${Boost_LIBRARIES}
                                  ${CRYPTO++_LIBRARIES})