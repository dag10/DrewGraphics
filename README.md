# HelloGlfw

This is a second project as I teach myself OpenGL using GLFW using [this website](https://learnopengl.com) instead
of the previous website. And instead of using Xcode, I'm using CMake, which makes it easier to build for any platform.

I'm still using git submodules to pull in the depdendencies.

## Organization

This HelloGlfw project was initially going to be a rather straight-forward following of the tutorial linked above, but
I've ended up taking several side steps in abstracting large chunks of it. Most notably, I created a "scene" system so
that I could take a large side step and implement a Portal-like system. As such, there are two named scenes:

- **"tutorial"** - Following along with the tutorial, implemented in [src/TutorialScene.cpp](./src/TutorialScene.cpp).
- **"portal"** - First-person camera in a room with portals you can fly through, implemented in
                 [src/PortalScene.cpp](./src/PortalScene.cpp).
                 
The scene is chosen as the first command-line argument. So to launch the app with a portal renderer, the command would be:

```
$ ./HelloGlfw portal
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
$ ./HelloGlfw tutorial
```

