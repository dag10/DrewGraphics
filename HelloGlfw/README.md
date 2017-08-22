# HelloGlfw

This is a second project as I teach myself OpenGL using GLFW using [this website](https://learnopengl.com) instead
of the previous website. And instead of using Xcode, I'm using CMake, which makes it easier to build for any platform.

I'm still using git submodules to pull in the depdendencies.

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
$ ./HelloGlfw
```

