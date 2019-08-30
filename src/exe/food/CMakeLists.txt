include_directories(../../lib)

set(SRC main.cpp)

add_executable(food ${SRC})
target_link_libraries(food  btree)