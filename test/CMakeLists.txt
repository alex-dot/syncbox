add_test(NAME hash_structors COMMAND flocksyTest -t hash_structors)
add_test(NAME hash_makeHash COMMAND flocksyTest -t hash_makeHash)
add_test(NAME hash_compare COMMAND flocksyTest -t hash_compare)
add_test(NAME hash_tree_size_compare COMMAND flocksyTest -t hash_tree_size_compare)
# add_test(NAME hash_tree_size_compare_random COMMAND flocksyTest -t hash_tree_size_compare_random)
# add_test(NAME hash_tree_compare COMMAND flocksyTest -t hash_tree_compare)
add_test(NAME hash_tree_compare_diff COMMAND flocksyTest -t hash_tree_compare_diff)
add_test(NAME hash_tree_sort COMMAND flocksyTest -t hash_tree_sort)
add_test(NAME hash_tree_top_hash COMMAND flocksyTest -t hash_tree_top_hash)
add_test(NAME directory_manual COMMAND flocksyTest -t directory_manual)
add_test(NAME directory_auto COMMAND flocksyTest -t directory_auto)
add_test(NAME directory_compare COMMAND flocksyTest -t directory_compare)
# add_test(NAME box_test COMMAND flocksyTest -t box_test)
add_test(NAME box_compare COMMAND flocksyTest -t box_compare)


include_directories(../include)
add_executable(flocksyTest testMain.cpp
                           ../src/constants.cpp
                           ../src/hash.cpp
                           ../src/hash_tree.cpp
                           ../src/directory.cpp
                           ../src/transmitter.cpp
                           #../src/box.cpp
                           #../src/boxconfig.cpp
                           test_hash.cpp
                           test_hash_tree.cpp
                           test_directory.cpp
                           #test_box.cpp
                           )
target_link_libraries(flocksyTest ${CMAKE_THREAD_LIBS_INIT}
                                  ${Boost_LIBRARIES}
                                  ${LIBSODIUM_LIBRARY}
                                  ${ZEROMQ_LIBRARY}
                                  ${ZEROMQ_CPP_LIBRARY}
                                  ${JSONCPP_LIBRARY})