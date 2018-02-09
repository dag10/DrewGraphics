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

| Scene | Description |
| :-- | :-- |
| **"tutorial"** | Following along with the tutorial, implemented in [src/scenes/TutorialScene.cpp](./src/scenes/TutorialScene.cpp).|
| **"portal"** | First-person camera in a room with portals you can fly through, implemented in [src/scenes/PortalScene.cpp](./src/scenes/PortalScene.cpp).<br><br>Here's a video of the portal project: https://fb.com/1925626090797272 |
| **"quad"** | A scene with just a single quad in front of the period, rotating to look at the cursor. I use it to test out new materials as I create them. Implemented in [src/scenes/QuadScene.cpp](./src/scenes/QuadScene.cpp). |
| **"meshes"** | A scene showing the few procedural meshes I've created, including cubes, a cylinder, and a cone. Implemented in [src/scenes/MeshesScene.cpp](./src/scenes/MeshesScene.cpp). |
| **"vr"** | A scene that creates a VR environment I'm developing as I learn the SteamVR API. Implemented in [src/scenes/VRScene.cpp](./src/scenes/VRScene.cpp).<br><br>Note: You can build the VRDemo configuration to default to this scene.|

The scene is chosen as the first command-line argument. So to launch the app with a portal renderer, the command would be:

```
$ ./DrewGraphics portal
```

## Getting started

The first thing you need to so is checkout the submodules:

```
$ git submodule update --init
```

### Creating the GLFW Project on Windows

If you're not building on Windows, you can skip this step. Once you checkout the submodules by following the step above, go into the _external/glfw_ directory. Create a new directory called _build_ (this name is required). Inside of build, run the command `cmake ..`, which will generate a Visual Studio project for GLFW. No need to build this project yourself, it'll be included in the DrewGraphics.sln solution.

## Building

### Windows

Open the DrewGraphics.sln project, then build and run.

### macOS

To build using CMake, create a new folder (e.g. _build_), go into that folder, run cmake on the project directory, then make
the resulting makefile. For example:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./DrewGraphics tutorial
```
