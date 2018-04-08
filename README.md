# DrewGraphics

This repo contains a personal graphics engine and projects built using the engine. The purpose
of this engine is to serve as a sandbox while I learn graphics techniques. It's absolutely
not intended for any sort of production use.

It originally started as a purely OpenGL engine while I followed
[this website's guides](https://learnopengl.com). As I took a DirectX course at my university,
I added in DirectX 11 as an optional graphics API.

## Organization

There are three main projects in their respective subdirectories.

**Engine** is the core engine, compiled to a library.

**Experiments** is a set of [scenes](./Experiments/src/scenes) (think *Unity* scenes) which
are essentially mini-projects. Scenes are chosen during runtime by passing in the scene name
as the launch argument, or typing it into standard input.

Some scenes have a VR variant (designated by the -vr suffix). These scenes rely on an
OpenVR-supported headset being connected.

**CaVR** is a simple proof-of-concept VR game I'm developing with the engine. In it, a small
spaceship attached to one of the VR controllers has to navigate an infinite and ever-shrinking
cave while collecting fuel cells and avoiding collision with the walls.

## Getting started

The first thing you need to do is checkout the submodules:

```
$ git submodule update --init
```

## Building

### Windows

Open the DrewGraphics.sln project, then build and run.

There are four configurations:

| Configuration | Description        |
| :--           | :--                |
| D_GL          | OpenGL Debug       |
| R_GL          | OpenGL Release     |
| D_DX          | DirectX 11 Debug   |
| R_DX          | DirectX 11 Release |

### macOS

To build using CMake, create a new folder (e.g. _build_), go into that folder, run cmake on the project directory, then make
the resulting makefile. Do this from the top-most directory. For example:

```
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./Experiments/Experiments
```
