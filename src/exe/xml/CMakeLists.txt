include_directories(../../lib)

set(SRC main.cpp)

add_executable(xml ${SRC})
target_link_libraries(xml  btree)