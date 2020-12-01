cmake_minimum_required(VERSION 3.17)
project(Crosser VERSION 0.4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SFML_INCLUDE_DIR "SFML/include")
set(SFML_LIBRARY_DIR "SFML/build/lib")

link_directories(SFML_LIBRARY_DIR)
include_directories(SFML_INCLUDE_DIR)

set(SFML_DIR "/Users/arnedalhuisen/Downloads/SFML-2.5.1/build")

find_package(SFML COMPONENTS system window graphics network audio REQUIRED)

configure_file(config.hpp.in config.hpp)
add_executable(Crosser main.cpp game.cpp utils.cpp)

target_link_libraries(Crosser sfml-graphics sfml-window sfml-system)
target_include_directories(Crosser PUBLIC "${PROJECT_BINARY_DIR}")