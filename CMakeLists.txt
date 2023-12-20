cmake_minimum_required(VERSION 3.27)
project(Game1)

set(CMAKE_CXX_STANDARD 23)

add_executable(Game1 main.cpp)

set(SFML_STATIC_LIBRARIES TRUE)
set(SFML_DIR C:/SFML/lib/cmake/SFML)
find_package(SFML COMPONENTS system window graphics audio network REQUIRED)

include_directories(C:/SFML/include/SFML)
target_link_libraries(Game1 sfml-system sfml-window sfml-graphics sfml-audio)


