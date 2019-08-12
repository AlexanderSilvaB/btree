include_directories(.)

find_package (Threads)
find_package( OpenCV )

if(OpenCV_FOUND)
    add_definitions(-DHAS_OPENCV)
endif()

set(SRC 
        Node.cpp
        ParallelPool.cpp
        BT.cpp
        Blackboard.cpp
        Var.cpp)

add_library(btree ${SRC})
target_link_libraries(btree ${CMAKE_THREAD_LIBS_INIT})
if(OpenCV_FOUND)
    target_link_libraries( btree ${OpenCV_LIBS} )
endif()