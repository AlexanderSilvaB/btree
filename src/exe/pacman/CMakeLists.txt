include_directories(../../lib)

set(SRC main.cpp)

add_executable(pacman ${SRC})
target_link_libraries(pacman  btree)