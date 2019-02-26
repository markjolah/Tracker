# Tracker CMakeLists.txt
# Main CMakeLists.txt for Tracker libraries


file(GLOB SRCS *.cpp)  #Gather all .cpp sources

include(AddSharedStaticLibraries)
# add_shared_static_libraries()
# * Add shared and static library targets to project namespace
# * Sets target_include_directories
# * Sets target_compile_features and target_link_libraries
# * Installs target libraries and headers
add_shared_static_libraries(SOURCES ${SRCS}
                            COMPILE_FEATURES cxx_std_11
                            PUBLIC_HEADER_DIR ${CMAKE_SOURCE_DIR}/include
                            RETURN_TARGETS lib_targets)

#Custom target settings for each lib_target from add_shared_static_libraries
foreach(target IN LISTS lib_targets)
    target_link_libraries(${target} PUBLIC BacktraceException::BacktraceException)
    target_link_libraries(${target} INTERFACE Armadillo::Armadillo)
endforeach()
