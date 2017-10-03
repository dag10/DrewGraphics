# DrewGraphics

This is an OpenGL-based 3D library/engine I'm building for myself as I learn
OpenGL. It has a "scene" system so that it's effectively several separate
programs in one. This allows me to follow along with [this tutorial](https://learnopengl.com)
and also occasionally diverge into totally separate projects, such as a portal
renderer.

## Organization

This project was initially going to be a rather straight-forward following of the tutorial linked above, but
I've ended up taking several steps to abstract large chunks of it. The largest
abstraction is the aformentioned scene system. There are currently two named
scenes:

- **"tutorial"** - Following along with the tutorial, implemented in [src/scenes/TutorialScene.cpp](./src/scenes/TutorialScene.cpp).
- **"portal"** - First-person camera in a room with portals you can fly through, implemented in
                 [src/scenes/PortalScene.cpp](./src/scenes/PortalScene.cpp).
                 
The scene is chosen as the first command-line argument. So to launch the app with a portal renderer, the command would be:

```
$ ./DrewGraphics portal
```

## Getting started

The first thing you need to so is checkout the submodules:

```
$ git submodule update --init
```

## Building

To build using CMake, create a new folder (e.g. _build_), go into that folder, run cmake on the project directory, then make
the resulting makefile. For example:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./DrewGraphics tutorial
```

