# Tracker CMakeLists.txt
# Mark J. Olah [mjo@cs.unm.edu] 2018

# set_property(DIRECTORY APPEND PROPERTY INCLUDE_DIRECTORIES ${CMAKE_CURRENT_SOURCE_DIR})

if(OPT_MATLAB)
    file(GLOB SRCS *.cpp)  #Gather all .cpp sources
    foreach(src ${SRCS})
        get_filename_component(target "${src}" NAME_WE)
        message("MakingMex: ${target}")
        mexiface_make_mex(MEXNAME ${target} SOURCES ${src}
                          PUBLIC_LINK_LIBRARIES Tracker::Tracker)
    endforeach()
endif()
