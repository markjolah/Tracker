# Tracker CMakeLists.txt
# Mark J. Olah [mjo@cs.unm.edu] 2018


if(OPT_MATLAB)
    file(GLOB SRCS *.cpp)  #Gather all .cpp sources
    foreach(src ${SRCS})
        get_filename_component(target "${src}" NAME_WE)
        message("MakingMex: ${target}")
        make_mex(${target})
        target_link_libraries(${target} Tracker::Tracker) #Link to main C++ library
    endforeach()
endif()
