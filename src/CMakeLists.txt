# Tracker CMakeLists.txt
# Mark J. Olah [mjo@cs.unm.edu] 2018

file(GLOB SRCS *.cpp)  #Gather all .cpp sources

include(AddSharedStaticLibraries)
# add_shared_static_libraries()
# * Add shared and static library targets to project namespace
# * Sets target_include_directories
# * Sets target_compile_features and target_link_libraries
# * Installs target libraries and headers
# * returns lsit SHARED_STATIC_LIB_TARGETS of created targets for adjusting additional target properties
add_shared_static_libraries(SOURCES ${SRCS}
                            PUBLIC_COMPILE_FEATURES cxx_std_11
                            PUBLIC_INCLUDE_DIRECTORIES ${ARMADILLO_INCLUDE_DIRS}
                            PUBLIC_HEADER_DIR ${CMAKE_SOURCE_DIR}/include
                            PUBLIC_DEBUG_HEADER_DIR $${CMAKE_SOURCE_DIR}/include/debug)

foreach(target ${SHARED_STATIC_LIB_TARGETS})
    #avoid having to modify old LAP_JVSparse code for ussed variable problems
    target_compile_options(${target} INTERFACE $<$<CXX_COMPILER_ID:GNU>:-Wno-unused-parameter>)
endforeach()
