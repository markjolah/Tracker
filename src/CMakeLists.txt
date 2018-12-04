# Tracker CMakeLists.txt
# Mark J. Olah [mjo@cs.unm.edu] 2018

file(GLOB SRCS *.cpp)  #Gather all .cpp sources
set(PUBLIC_HEADER_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/../include)

include(AddSharedStaticLibraries)
add_shared_static_libraries("${SRCS}" ${PROJECT_NAME}) 

foreach(target ${LIB_TARGETS})
    target_compile_features(${target} PUBLIC cxx_std_14) #C++14 required for variadic templates support.
    target_compile_options(${target} INTERFACE $<$<CXX_COMPILER_ID:GNU>:-Wno-unused-parameter>)
    target_include_directories(${target} PUBLIC $<BUILD_INTERFACE:${PUBLIC_HEADER_SRC_DIR}> 
                                                $<INSTALL_INTERFACE:include>)
    target_link_libraries(${target} PUBLIC ${ARMADILLO_LIBRARIES})
endforeach()

install(TARGETS ${LIB_TARGETS} EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION lib COMPONENT Runtime
        ARCHIVE DESTINATION lib COMPONENT Development)

install(DIRECTORY ${PUBLIC_HEADER_SRC_DIR}/ DESTINATION include COMPONENT Development)
