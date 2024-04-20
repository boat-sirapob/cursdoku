cmake_minimum_required(VERSION 3.10)
project(cursedoku)

find_package(Curses REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(include)
file(GLOB SRC src/*.c)

add_executable(cursedoku ${SRC})

target_include_directories(cursedoku PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(cursedoku ncurses)
