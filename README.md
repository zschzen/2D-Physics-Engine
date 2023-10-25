# 2d Physics Engine

A study project that focuses on the development of a 2D physics engine using C++, SDL, and CMake. The engine was created during my time attending the [Pikuma] class, where we explored the world of physics simulations, utilizing rigid bodies and constraint-based solutions.

The purpose of this project is to provide a practical demonstration of physics concepts in the context of game development and simulation. While attending Pikuma class, we delved into various physics scenarios, and this repository is the result of our dedicated efforts.

---

### Features

-   **Rigid-Body**: A simple rigid-body physics engine created from scratch.
-   **Physics Concepts**: Includes concepts like velocity, acceleration, integration, mass, forces, gravity, drag, friction, rigid body dynamics, collision detection, constraints, etc.
-   **Shapes and Collision**: Works with rigid bodies by adding shapes to objects like circles, rectangles, and polygons. Includes collision detection and resolution.
-   **Constraints**: Adds constraints to the physics engine for objects like joints and ragdolls.

For more details about the course, please visit the [2D Game Physics Programming].


## Compiling

This project uses [CMake] as its build system. Here are the steps to compile it:

1.  Navigate to the project directory.
2.  Create a new directory for the build files:
    
    ```
    mkdir build
    cd build
    ```
    
3.  Run CMake to generate the build files:
    
    ```
    cmake ..
    ```
    
4.  Compile the project:
    
    ```
    make
    ```
    

### Compiling with Emscripten

If you want to compile this project to WebAssembly using [Emscripten], you can follow these steps:

1.  Make sure you have Emscripten installed and sourced in your terminal.
2.  Navigate to the project directory.
3.  Create a new directory for the Emscripten build files:
    
    ```
    mkdir emscripten_build
    cd emscripten_build
    ```
    
4.  Run Emscripten’s CMake to generate the build files:
    
    ```
    cmake -H . -B build -G Ninja "-DCMAKE_TOOLCHAIN_FILE=<fullpath_to_emsdk>/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake"
    ```
    
5.  Compile the project:
    
    ```
    make
    ```

-----
![readme version](https://img.shields.io/badge/%2F~.-lightgrey.svg?style=flat-square&colorA=808080&colorB=808080)![readme version](https://img.shields.io/badge/17%2F09%2F23--lightgrey.svg?style=flat-square&colorA=000000&colorB=808080)

**Live long and prosper \V/,**


[//]: # (External links)

[pikuma]: <https://pikuma.com>
[2D Game Physics Programming]: <https://pikuma.com/courses/game-physics-engine-programming>
[cmake]: <https://cmake.org>
[emscripten]: <https://emscripten.org>
