# VKING
## A high-performance, module-first game/graphics engine

![Status](https://img.shields.io/badge/status-work%20in%20progress-red)
![C++](https://img.shields.io/badge/C++-23-blue)
![License](https://img.shields.io/badge/license-GPL%20v3%20with%20linking%20exceptions-blue)

## Current Status
**Work in Progress** - does not render yet

### Completed
- **CMake Build System**: Supports cross-platform compilation and integration.
- **Logging**: Logging library for easy logging of debug information (See src/Shared/src/VKING/Log.ixx)
- **Engine Controlled Lifecycle System**: Engine controls its own entry point and lifecycle. Caller provides two methods (`void VKING::registerLogger()`, and `void* VKING::createApplication()` where void* must impliment the VKING::Application trait)
- **Signal Handling and Controlled Shutdown**: Handles signals (SIGINT, SIGTERM) on POSIX systems, incomplete implementation on Windows. This triggers a safe shutdown. Implimentation is `async-signal-safe` to the maximum extent possible, using std::atomic.

## Key Features (Planned)
- **Modular Architecture**: Utilizes C++ Modules to allow for easy extension, customization, portability, and compile time optimizations.
- **Vulkan API**: Uses the Vulkan API for graphics rendering (and possibly other backends in the future)
- **Multi-threaded**: Built from the ground up to be multi-threaded.
- **Cross-Platform**: Built primarily for MacOS with MoltenVK, but written for 1.5st class cross-platform support.
- **Fast**: Built with performance and modern C++ features in mind.
- **Rich Build System**: Supports CMake for easy integration and customization.

## Purpose
This project is meant to be a learning experience for myself, and a career showpiece.
It is not intended to be a production-ready engine, though it may at some point become one.
The project is designed to be modular, performant, and cross-platform, with a focus on learning and experimentation.
This means unusual C++ features and techniques are used, including modules, coroutines, and more.
It is also designed to be easy to extend and customize and has an easy-to-use API.

## Minimum Requirements
- C++23 Compiler (or later)
- Module Support
- Support for all requirements of GLM, Vulkan SDK, GLFW, spdlog, and fmt on your respective platforms (which may vary)
- Vulkan SDK - Vulkan support is required at this time. The engine can be compiled without via cmake options, but will not run.

## Getting Started
Clone the repo, and run standard CMake commands.
If they error during the initial cmake cache generation, it should specify what component is missing on your system (for example, libx11-dev on Ubuntu or whatever)
All dependencies should be cloned from GitHub automatically.

```bash
# Generally, follow this format
mkdir build && cd build
cmake ..

make . # or whatever your build system is
```
## Common Issues
- **Apple**: Apple Clang has almost no or no support at all for C++ modules. Please install GCC or LLVM.
- **LLVM@21**: LLVM 21 has problems building spdlog. Please use LLVM 20 or earlier, or LLVM 22 or later once it comes out.
- **Windows**: Unknown at this time
- **Linux**: Unknown at this time, likely the same as Apple since they are both POSIX compliant
- **Missing Libraries**: CMake tells you when stuff isn't found. Please install the missing libraries.

## License
This project is licensed under the GPL v3, with linking exceptions as detailed in the license file. 
This specifically means that you can use this project in your own builds under any license,
but the *library* code itself remains under GPL.
(i.e. don't fork and make closed source, but feel free to make closed source games with it)