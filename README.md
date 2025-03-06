# Fluid Simulation Project

A standalone C++ fluid simulation with a custom renderer, no external graphics libraries required.

## Overview

This project implements a real-time fluid simulation based on Jos Stam's "Stable Fluids" algorithm. The simulation solves the Navier-Stokes equations using a semi-Lagrangian approach for numerical stability. The implementation is purely in C++ without any external graphics libraries - all visualization is done through a custom renderer that outputs frames as PPM image files.

## Features

- Grid-based fluid simulation
- Velocity and density fields
- Diffusion, advection, and projection steps
- Custom visualization without external dependencies
- Frame-by-frame PPM image output
- FPS counter for performance monitoring
- **New**: Frame limit option for controlled video length
- **New**: One-step simulation to video conversion

## Project Structure

- `main.cpp` - Entry point, simulation loop
- `FluidSimulator.h/cpp` - Core fluid simulation algorithm
- `Renderer.h/cpp` - Custom rendering system
- `build.bat` - Windows build script for MSVC
- `convert_to_video.bat` - Script to convert frames to video using FFmpeg
- `run_sim_to_video.bat` - **New**: Combined simulation and video generation
- `CMakeLists.txt` - Alternative CMake build system
- `.vscode/` - VS Code configuration files

## Building and Running

### Using MSVC (cl.exe)

```batch
# Build the project
build.bat

# Build and run the simulation with default settings (unlimited frames)
build.bat run

# Build and run with a specific frame count
build.bat run 300
```

Note: The build script has been simplified for maximum compatibility. Make sure to run it from a Developer Command Prompt for Visual Studio.

### Using the Combined Simulation and Video Tool

```batch
# Generate 300 frames at 30fps (defaults)
run_sim_to_video.bat

# Generate 100 frames at 30fps
run_sim_to_video.bat 100

# Generate 200 frames at 60fps
run_sim_to_video.bat 200 60

# Generate 150 frames at 24fps with custom filename
run_sim_to_video.bat 150 24 my_animation
```

### Using CMake with MSVC

```batch
# Create build directory
mkdir build
cd build

# Configure and build with MSVC
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release

# Run the simulation
cd ..
.\build\Release\fluid_sim.exe
```

## VS Code Integration

This project includes full VS Code configuration:

- `settings.json` - Editor and C++ settings
- `tasks.json` - Custom build tasks
- `launch.json` - Debug configuration
- `c_cpp_properties.json` - C/C++ configuration

To build and run in VS Code:
1. Open the command palette (Ctrl+Shift+P)
2. Select "Tasks: Run Build Task" to build the project
3. Select "Tasks: Run Test Task" to run the simulation

## Video Output with FFmpeg

The simulation outputs frames as PPM files in the `frames` directory. You can convert these to a video using FFmpeg.

### Using the Conversion Script

```batch
# Convert frames to a video with default 30fps
convert_to_video.bat

# Convert frames with a specific framerate
convert_to_video.bat 60

# Convert frames with specific framerate and output name
convert_to_video.bat 60 my_fluid_video
```

### Using VS Code Task

1. Press `Ctrl+Shift+P`
2. Type "Tasks: Run Task"
3. Select "Convert Frames to Video"

### Manual FFmpeg Command

```batch
ffmpeg -framerate 30 -i frames/frame_%04d.ppm -c:v libx264 -pix_fmt yuv420p -crf 23 output.mp4
```

## Troubleshooting

### MSVC Compiler Issues

- Make sure to run build.bat from a Developer Command Prompt for Visual Studio
- If you encounter build errors, try using the direct build commands:
  ```
  mkdir build
  mkdir frames
  cl.exe /nologo /EHsc /W4 /std:c++17 /O2 /MD /I. /Fo:build/ /Fe:fluid_sim.exe main.cpp FluidSimulator.cpp Renderer.cpp /link /SUBSYSTEM:CONSOLE
  ```

### FFmpeg Issues

- Ensure FFmpeg is installed and added to your PATH
- Test your FFmpeg installation by running `ffmpeg -version` in a command prompt
- Make sure your simulation has run and generated PPM files before trying to convert

## License

This project is released under the MIT License.