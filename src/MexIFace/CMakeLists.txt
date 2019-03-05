# src/MexIFace/CMakeLists.txt
# MexIFace Matlab module(s)

# set_property(DIRECTORY APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})

if(OPT_MATLAB)
    file(GLOB SRCS *.cpp)  #Gather all .cpp sources
    foreach(src IN LISTS SRCS)
        get_filename_component(target ${src} NAME_WE)
        message(STATUS "MakingMexModule: ${target}")
        mexiface_make_mex(MEXNAME ${target} SOURCES ${src}
                         LINK_LIBRARIES Tracker::Tracker)
    endforeach()
endif()
