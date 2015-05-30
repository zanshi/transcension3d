# Transcension3D

Transcension3D is a 3D game engine written in C++ and OpenGL3.3. It was made by six guys from [LiTH](https://www.liu.se/?l=en&sc=true), Sweden, during a university course in computer graphics. The core architecture is built around the Entity-Component-System pattern as implemented by the excellent open source library [EntityX](https://github.com/alecthomas/entityx/).

## Engine features

 - Physics simulation and continuous collision detection using [Bullet](http://bulletphysics.org/)
 - Controls using keyboard or console controller (PS4 and Xbox360 tested)
 - Level editing in [Blender](http://www.blender.org/), see our [Wiki](https://github.com/zanshi/swag3d/wiki/Level-Editing-in-Blender) for details
 - Dynamic lighting and shadows for point lights using shadow mapping (in development)
 - Multi-platform support: Mac, Windows and Linux

## Transcension - the game

As of now the engine codebase includes the game we built using the engine - Transcension (it's where we got the name of the engine from, d'uh). It is a First Person Puzzling game (FPP?!) in which the player has the ability to move between two parallel worlds, i.e. _transcend_ between dimensions. This allows the player to overcome obstacles present in one dimension by passing to the other dimension where the level looks slightly different. 

## Installation / Requirements

### All platforms
 - CMake
 - Git

### Windows
 - [Mingw32](http://sourceforge.net/projects/mingw/)

or

- Visual Studio 2013

To generate a Visual Studio 2013 solution, go into the visualc directory, open a terminal window and type
```
cmake .. -G "Visual Studio 12 2013"
```

If you want MinGW makefiles again, type
```
cmake .. -G "MinGW Makefiles"
```
### OSX
 - [SDL 2.0.3 Dev Libs](http://www.libsdl.org/download-2.0.php), Clang or GCC

Tested with CLion and Cmake + make in the terminal

### Linux
 - SDL

## Authors:
- [Oscar Nord](https://github.com/Furbee)
- [Johan Nordin](https://github.com/johanordin)
- [Niclas Olmenius](https://github.com/zanshi)
- [Adrian Palmquist](https://github.com/adrianpalmquist)
- [Cristoffer Särevall](https://github.com/saerevall)
- [Benjamin Wiberg](https://github.com/bwiberg)
