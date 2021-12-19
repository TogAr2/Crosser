cmake_minimum_required(VERSION 3.17)
project(Crosser VERSION 2.4)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ldl -lpthread")

set(CROSSER_PLUGIN_FOLDER "./plugins/")

find_package(SFML COMPONENTS system window graphics network audio REQUIRED)
find_package(CrosserAPI REQUIRED)

configure_file(main/config.hpp.in config.hpp)
add_executable(Crosser main/main.cpp game/game.cpp game/tile.cpp game/tile.hpp plugins/pluginManager.cpp plugins/pluginManager.hpp multiplayer/network.cpp multiplayer/network.hpp multiplayer/player.cpp multiplayer/player.hpp render/hud.hpp render/hud.cpp render/gui.cpp render/gui.hpp render/render.cpp render/render.hpp render/screens/mainScreen.cpp render/screens/mainScreen.hpp render/renderUtils.cpp render/renderUtils.hpp render/screens/multiplayerScreen.cpp render/screens/multiplayerScreen.hpp)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/resources)

target_link_libraries(Crosser PUBLIC CrosserAPI sfml-graphics sfml-window sfml-system sfml-network)

target_include_directories(Crosser PUBLIC "${PROJECT_BINARY_DIR}")
target_include_directories(Crosser PUBLIC "${CMAKE_INSTALL_FULL_INCLUDEDIR}/CrosserAPI")