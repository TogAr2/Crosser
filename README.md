# Crosser
Crosser is a simple 2D game in which you have to find the cross (actually a green square right now).

## Install the API
Before you build Crosser, you must have the API installed. Look at _"Building the API"_ in the readme of [CrosserAPI](https://github.com/Bloepiloepi/Crosser-API).

## How to build
- Make sure you have homebrew installed
- Make sure you have sfml and cmake installed using homebrew
```shell script
brew install sfml
brew install cmake
```
- Clone this repository
- Cd to the project root
- Execute this command to build, replacing ```/path/to/target/folder``` with any folder you want to use (**important:** don't use the same folder as the API):
```shell script
cmake -S . -B /path/to/target/folder; cmake --build /path/to/target/folder --target Crosser -- -j 3;
```
You can now run Crosser using ```./Crosser```!