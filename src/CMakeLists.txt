add_library(core
    filter.c
    filter.h
    
    image.c
    image.h
    
    threadpool.c
    threadpool.h
        
    processing.c
    processing.h
    
    list.c
    list.h
        
    utils.c
    utils.h
    
    cwalk.c
    cwalk.h
    
    crc32.cpp
    crc32.h
    crc32.hpp
)
target_link_libraries(core PUBLIC png)
target_include_directories(core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(ieffect ieffect.c)
target_link_libraries(ieffect PRIVATE core)
