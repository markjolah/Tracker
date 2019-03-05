# src/CMakeLists.txt
# Tracker - main libraries

file(GLOB SRCS *.cpp)  #Gather all .cpp sources

# add_shared_static_libraries()
# * Add shared and static library targets to project namespace
# * Installs target libraries and headers
# * returns target list for further processing
include(AddSharedStaticLibraries)
add_shared_static_libraries(SOURCES ${SRCS}
                            COMPILE_FEATURES cxx_std_11
                            PUBLIC_HEADER_DIR ${CMAKE_SOURCE_DIR}/include
                            RETURN_TARGETS lib_targets)

#Custom target settings for each lib_target created by add_shared_static_libraries()
foreach(target IN LISTS lib_targets)
    target_link_libraries(${target} PUBLIC BacktraceException::BacktraceException)
    target_link_libraries(${target} INTERFACE Armadillo::Armadillo)
endforeach()
