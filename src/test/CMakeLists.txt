
# test/CMakeLists.txt
# CTest integration of googletest

set(TEST_EXE test_lap)
file(GLOB TEST_SRCS *.cpp)
add_executable(${TEST_EXE} ${TEST_SRCS})
target_link_libraries(${TEST_EXE} ${PROJECT_NAME}::${PROJECT_NAME})
target_include_directories(${TEST_EXE} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
add_test(NAME ${TEST_EXE} COMMAND ${TEST_EXE})

# target_link_libraries(${GTEST_EXE} GTest::GTest GTest::Main)
# target_link_libraries(${GTEST_EXE} ${ARMADILLO_LIBRARIES})

# add_executable(debug_bvn debug_bvn.cpp)
# target_link_libraries(debug_bvn ${PROJECT_NAME}::${PROJECT_NAME}) 

