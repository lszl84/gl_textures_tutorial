# gl_textures_tutorial
The basics of textures in OpenGL

## Requirements

This works on Windows, Mac, and Linux. You'll need `cmake` and a C++ compiler (tested on `clang`, `gcc`, and MSVC).

OpenGL tutorials require OpenGL development package on Linux (Windows and Mac should work out of the box):

```sh
sudo apt install libglu1-mesa-dev
```

## Building

To build the project, use:

```bash
cmake -S. -Bbuild
cmake --build build
```

This will create a directory named `build` and create all build artifacts there.
