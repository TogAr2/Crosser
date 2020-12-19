cmake_minimum_required(VERSION 3.17)
project(Crosser VERSION 1.2)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CROSSER_PLUGIN_FOLDER "./plugins/")

find_package(SFML COMPONENTS system window graphics network audio REQUIRED)
find_package(CrosserAPI REQUIRED)

configure_file(config.hpp.in config.hpp)
add_executable(Crosser main.cpp game.cpp utils.cpp tile.cpp tile.hpp pluginManager.cpp pluginManager.hpp)

target_link_libraries(Crosser PUBLIC CrosserAPI sfml-graphics sfml-window sfml-system)

target_include_directories(Crosser PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(Crosser PUBLIC "${CMAKE_INSTALL_FULL_INCLUDEDIR}/CrosserAPI")