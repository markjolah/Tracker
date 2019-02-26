# Tracker/test/CMakeLists.txt

set(TEST_TARGET test${PROJECT_NAME})
file(GLOB TEST_SRCS *.cpp)
add_executable(${TEST_TARGET} ${TEST_SRCS})
target_link_libraries(${TEST_TARGET} ${PROJECT_NAME}::${PROJECT_NAME})
set_target_properties(${TEST_TARGET} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})

if(OPT_INSTALL_TESTING)
    if(WIN32)
        set(TESTING_INSTALL_DESTINATION bin)
    elseif(UNIX)
        set(TESTING_INSTALL_DESTINATION lib/${PROJECT_NAME}/test)
        set_target_properties(${TEST_TARGET} PROPERTIES INSTALL_RPATH "\$ORIGIN/../..")
    endif()
    install(TARGETS ${TEST_TARGET} RUNTIME DESTINATION ${TESTING_INSTALL_DESTINATION} COMPONENT Testing)
endif()

