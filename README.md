# Crosser
Crosser is a simple 2D game in which you have to find the cross (actually a green square right now).

## How to build
- Make sure you have homebrew installed
- Make sure you have sfml and cmake installed using homebrew
```shell script
brew install sfml
brew install cmake
```
- Clone this repository
- Cd to the project root
- Execute this command to build, replacing ```/path/to/target/folder``` with any folder you want to use:
```shell script
cmake -S . -B /path/to/target/folder; cmake --build /path/to/target/folder --target Crosser -- -j 3;
```
You can now run Crosser using ```./Crosser```!