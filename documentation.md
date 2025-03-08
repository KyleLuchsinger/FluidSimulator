# Fluid Simulation Codebase Documentation

This document provides a detailed explanation of the fluid simulation codebase, which implements Jos Stam's "Stable Fluids" algorithm for simulating fluid dynamics. The project consists of a C++ implementation that simulates fluid behavior and renders the output as a sequence of image frames.

## Table of Contents

1. [Overview](#overview)
2. [Project Structure](#project-structure)
3. [Core Components](#core-components)
   - [FluidSimulator](#fluidsimulator)
   - [Renderer](#renderer)
   - [Main Application](#main-application)
4. [Algorithm Details](#algorithm-details)
   - [Navier-Stokes Equations](#navier-stokes-equations)
   - [Semi-Lagrangian Approach](#semi-lagrangian-approach)
   - [Diffusion](#diffusion)
   - [Advection](#advection)
   - [Projection](#projection)
   - [Boundary Handling](#boundary-handling)
5. [Visualization Approach](#visualization-approach)
6. [Build System](#build-system)
7. [Video Output Process](#video-output-process)
8. [Performance Considerations](#performance-considerations)
9. [Extending the Codebase](#extending-the-codebase)

## Overview

This project implements a grid-based fluid simulation that solves the Navier-Stokes equations using a semi-Lagrangian approach. The simulation tracks both fluid density and velocity fields on a fixed grid, updating them over time to create realistic fluid motion. The output is visualized as a sequence of PPM image files that can be combined into a video.

Key features:
- Real-time fluid simulation with adjustable parameters
- Custom rendering without external dependencies
- Frame-by-frame output for video creation
- Performance monitoring with an FPS counter
- Configurable simulation length

## Project Structure

The codebase is organized into several key files:

- **Core Simulation:**
  - `FluidSimulator.h/cpp`: Implements the fluid simulation algorithm
  - `Renderer.h/cpp`: Handles visualization and frame output
  - `main.cpp`: Controls the simulation loop and initialization

- **Build System:**
  - `build.bat`: Windows build script for MSVC
  - `CMakeLists.txt`: Alternative CMake build system
  - `makefile`: Unix/Linux build configuration

- **Utility Scripts:**
  - `convert_to_video.bat`: Converts output frames to video using FFmpeg
  - `run_sim_to_video.bat`: Combined script for simulation and video generation

- **VS Code Configuration:**
  - `.vscode/`: Contains IDE settings for building and debugging

## Core Components

### FluidSimulator

The `FluidSimulator` class is the heart of the simulation, responsible for the numerical simulation of fluid dynamics.

#### Key Fields

```cpp
int size;         // Grid size
float dt;         // Time step
float diffusion;  // Diffusion rate
float viscosity;  // Viscosity

float* density;      // Density field
float* densityPrev;  // Previous density field

float* velocityX;      // Velocity field (x component)
float* velocityY;      // Velocity field (y component)
float* velocityPrevX;  // Previous velocity field (x component)
float* velocityPrevY;  // Previous velocity field (y component)
```

#### Core Methods

- **Constructor/Destructor**: Allocates and frees the necessary memory for simulation fields.
- **`step()`**: Advances the simulation by one time step, executing the diffusion, advection, and projection steps.
- **`addDensity(x, y, amount)`**: Adds fluid density at a specific grid location.
- **`addVelocity(x, y, amountX, amountY)`**: Adds velocity at a specific grid location.
- **`diffuse()`**: Handles fluid diffusion using Gauss-Seidel relaxation.
- **`project()`**: Enforces mass conservation (incompressibility) in the velocity field.
- **`advect()`**: Moves density and velocity through the grid based on current velocities.
- **`setBoundary()`**: Handles boundary conditions at the edges of the simulation.
- **`IX(x, y)`**: Helper function to convert 2D coordinates to a 1D array index.

### Renderer

The `Renderer` class handles the visualization of the fluid simulation, converting the simulation data into colored pixels and outputting frames.

#### Key Fields

```cpp
int width;        // Rendering width
int height;       // Rendering height
int gridSize;     // Simulation grid size
std::vector<Color> frameBuffer;  // Pixel buffer for rendering
int frameCount;   // Number of frames rendered
```

#### Core Methods

- **Constructor/Destructor**: Initializes the rendering system and frame output directory.
- **`clear()`**: Clears the frame buffer.
- **`renderFluid(simulator)`**: Renders the fluid simulation data to the frame buffer.
- **`display()`**: Outputs the current frame buffer as a PPM file.
- **`setPixel(x, y, color)`**: Sets a specific pixel in the frame buffer.
- **`densityToColor(density)`**: Maps fluid density values to colors using HSV to RGB conversion.
- **`writePPM(filename)`**: Writes the frame buffer to a PPM image file.

### Main Application

The `main.cpp` file ties everything together, creating instances of `FluidSimulator` and `Renderer`, setting up the initial conditions, and running the simulation loop.

#### Key Components

- **Configuration Constants**: Parameters like window size, grid size, time step, diffusion rate, etc.
- **Command-line Argument Parsing**: Allows configuring the maximum number of frames to generate.
- **Initial Conditions**: Sets up the initial fluid state with density and velocity.
- **Simulation Loop**: Repeatedly calls the simulator and renderer to advance the simulation and generate frames.
- **FPS Calculation**: Monitors and reports performance.

## Algorithm Details

### Navier-Stokes Equations

The simulation is based on the incompressible Navier-Stokes equations:

1. **Continuity Equation (Mass Conservation)**:
   ∇ · u = 0

2. **Momentum Equation**:
   ∂u/∂t + (u · ∇)u = -∇p/ρ + ν∇²u + F

Where:
- u is the velocity field
- p is pressure
- ρ is density
- ν is viscosity
- F represents external forces

Jos Stam's method solves these equations by splitting them into separate steps:

### Semi-Lagrangian Approach

The simulation uses a semi-Lagrangian approach for advection, which improves numerical stability even with larger time steps. This approach:

1. Considers where each fluid particle came from in the previous time step.
2. Uses bilinear interpolation to compute values at non-grid locations.
3. Ensures unconditional stability, allowing larger time steps.

### Diffusion

Diffusion is handled using an implicit method with Gauss-Seidel relaxation:

```cpp
void FluidSimulator::diffuse(int b, float* x, float* x0, float diff, float dt) {
    float a = dt * diff * (size - 2) * (size - 2);

    // Gauss-Seidel relaxation
    for (int k = 0; k < 20; k++) {
        for (int j = 1; j < size - 1; j++) {
            for (int i = 1; i < size - 1; i++) {
                x[IX(i, j)] = (x0[IX(i, j)] + a * (x[IX(i + 1, j)] + x[IX(i - 1, j)] +
                                                   x[IX(i, j + 1)] + x[IX(i, j - 1)])) /
                              (1 + 4 * a);
            }
        }
        setBoundary(b, x);
    }
}
```

This method iteratively solves a linear system to find the diffused values. The coefficient `a` combines the diffusion rate and time step for numerical stability. The method runs 20 iterations for convergence.

### Advection

Advection moves the fluid properties (density and velocity) through the grid based on the current velocity field:

```cpp
void FluidSimulator::advect(int b, float* d, float* d0, float* velocX, float* velocY, float dt) {
    float dt0 = dt * (size - 2);

    for (int j = 1; j < size - 1; j++) {
        for (int i = 1; i < size - 1; i++) {
            // Backtrack
            float x = i - dt0 * velocX[IX(i, j)];
            float y = j - dt0 * velocY[IX(i, j)];

            // Clamp to boundaries
            x = std::max(0.5f, std::min(size - 1.5f, x));
            y = std::max(0.5f, std::min(size - 1.5f, y));

            // Integer positions
            int i0 = static_cast<int>(x);
            int j0 = static_cast<int>(y);
            int i1 = i0 + 1;
            int j1 = j0 + 1;

            // Fractional positions for interpolation
            float s1 = x - i0;
            float s0 = 1 - s1;
            float t1 = y - j0;
            float t0 = 1 - t1;

            // Bilinear interpolation
            d[IX(i, j)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
                          s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
        }
    }

    setBoundary(b, d);
}
```

The advection step:
1. Traces particles backward in time based on velocity
2. Uses bilinear interpolation to compute values between grid points
3. Applies boundary conditions to the result

### Projection

The projection step ensures that the velocity field remains mass-conserving (divergence-free):

```cpp
void FluidSimulator::project(float* velocX, float* velocY, float* p, float* div) {
    // Calculate divergence of velocity field
    for (int j = 1; j < size - 1; j++) {
        for (int i = 1; i < size - 1; i++) {
            div[IX(i, j)] = -0.5f * (velocX[IX(i + 1, j)] - velocX[IX(i - 1, j)] +
                                     velocY[IX(i, j + 1)] - velocY[IX(i, j - 1)]) / size;
            p[IX(i, j)] = 0;
        }
    }

    // Solve for pressure field using Gauss-Seidel relaxation
    /* ... */

    // Subtract pressure gradient from velocity field
    for (int j = 1; j < size - 1; j++) {
        for (int i = 1; i < size - 1; i++) {
            velocX[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) * size;
            velocY[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) * size;
        }
    }
}
```

This step:
1. Computes the divergence of the velocity field
2. Solves a Poisson equation to find the pressure field
3. Subtracts the pressure gradient from the velocity field
4. Ensures the velocity field is divergence-free

### Boundary Handling

The `setBoundary` function enforces boundary conditions at the edges of the simulation grid:

```cpp
void FluidSimulator::setBoundary(int b, float* x) {
    // Handle the edges
    for (int i = 1; i < size - 1; i++) {
        x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
        x[IX(i, size - 1)] = b == 2 ? -x[IX(i, size - 2)] : x[IX(i, size - 2)];
    }

    for (int j = 1; j < size - 1; j++) {
        x[IX(0, j)] = b == 1 ? -x[IX(1, j)] : x[IX(1, j)];
        x[IX(size - 1, j)] = b == 1 ? -x[IX(size - 2, j)] : x[IX(size - 2, j)];
    }

    // Handle the corners
    x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
    x[IX(0, size - 1)] = 0.5f * (x[IX(1, size - 1)] + x[IX(0, size - 2)]);
    x[IX(size - 1, 0)] = 0.5f * (x[IX(size - 2, 0)] + x[IX(size - 1, 1)]);
    x[IX(size - 1, size - 1)] = 0.5f * (x[IX(size - 2, size - 1)] + x[IX(size - 1, size - 2)]);
}
```

The function:
1. Sets values for the edge cells based on their neighbors
2. Handles special cases for velocity components (b=1 for x-velocity, b=2 for y-velocity)
3. Ensures corners have consistent values using averages of adjacent cells

## Visualization Approach

The visualization approach converts simulation data into colorful imagery for rendering:

### Density to Color Mapping

The `densityToColor` method maps fluid density values to colors using an HSV to RGB conversion:

```cpp
Color Renderer::densityToColor(float density) const {
    // Map density to a color
    density = (std::min)(1.0f, (std::max)(0.0f, density / 50.0f));

    // HSV to RGB conversion for a nice color gradient
    float h = (1.0f - density) * 240.0f;  // Hue (0-360) - from red to blue
    float s = 1.0f;                       // Saturation
    float v = density;                    // Value

    // HSV to RGB conversion
    /* ... */

    return Color(static_cast<uint8_t>((r + m) * 255),
                 static_cast<uint8_t>((g + m) * 255),
                 static_cast<uint8_t>((b + m) * 255));
}
```

This creates a pleasing heat map effect:
- High density regions appear as bright red
- Medium density regions transition through yellow and green
- Low density regions fade to blue
- Zero density is rendered as black (transparent)

### Rendering Process

The rendering process in `renderFluid` method:
1. Scales the simulation grid to the output resolution
2. Maps density values to colors
3. Fills the frame buffer with the appropriate colors
4. Writes the frame buffer to a PPM file

## Build System

The project includes multiple build options for different environments:

### Windows Build with MSVC (build.bat)

```bat
cl.exe /nologo /EHsc /W4 /std:c++17 /O2 /MD /I. /Fo:build/ /Fe:fluid_sim.exe
       main.cpp FluidSimulator.cpp Renderer.cpp /link /SUBSYSTEM:CONSOLE
```

This script:
1. Creates necessary directories
2. Compiles the code with MSVC (Visual Studio Compiler)
3. Links the executable
4. Optionally runs the simulation

### CMake Build

```cmake
cmake_minimum_required(VERSION 3.10)
project(fluid_simulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(fluid_sim
    main.cpp
    FluidSimulator.cpp
    Renderer.cpp
)
```

This approach:
1. Works across multiple platforms
2. Configures appropriate compiler flags for different environments
3. Creates build files for the native build system (e.g., Make, Visual Studio)

### Unix/Linux Makefile

```makefile
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2

SRC = main.cpp FluidSimulator.cpp Renderer.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = fluid_sim
```

This provides a traditional Make-based build for Unix-like systems.

## Video Output Process

The simulation outputs individual PPM image files that are later combined into a video:

### PPM Image Format

PPM (Portable Pixmap) is a simple uncompressed image format:
- Plain text header with format, dimensions, and max color value
- Binary RGB data for each pixel

```cpp
void Renderer::writePPM(const char* filename) {
    std::ofstream file(filename, std::ios::binary);

    // Write the PPM header
    file << "P6\n" << width << " " << height << "\n255\n";

    // Write the pixel data
    for (const auto& pixel : frameBuffer) {
        file.write(reinterpret_cast<const char*>(&pixel.r), 1);
        file.write(reinterpret_cast<const char*>(&pixel.g), 1);
        file.write(reinterpret_cast<const char*>(&pixel.b), 1);
    }
}
```

### Video Conversion

The `convert_to_video.bat` script uses FFmpeg to convert the PPM frames to an MP4 video:

```bat
ffmpeg -y -framerate %FPS% -i frames/frame_%%04d.ppm -c:v libx264 -pix_fmt yuv420p -crf 23 %OUTPUT%
```

Key parameters:
- `-framerate %FPS%`: Sets the video frame rate
- `-i frames/frame_%%04d.ppm`: Input files pattern
- `-c:v libx264`: H.264 video codec
- `-pix_fmt yuv420p`: Compatible pixel format
- `-crf 23`: Quality setting (lower is better)

### All-in-One Script

The `run_sim_to_video.bat` combines simulation and video conversion:
1. Builds and runs the simulation for a specified number of frames
2. Converts the output frames to a video using FFmpeg
3. Allows customizing frame count, framerate, and output filename

## Performance Considerations

The simulation tracks and reports performance metrics:

```cpp
// FPS calculation
auto lastTime = std::chrono::high_resolution_clock::now();
int frameCount = 0;
int fpsCount = 0;

// Inside the main loop
auto currentTime = std::chrono::high_resolution_clock::now();
auto elapsedTime =
    std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count();

if (elapsedTime >= 1) {
    std::cout << "FPS: " << fpsCount / elapsedTime << " (Frame " << frameCount << "/"
              << maxFrames << ")" << std::endl;
    fpsCount = 0;
    lastTime = currentTime;
}
```

Performance is affected by:
- Grid size: Larger grids require more computation
- Iteration counts in Gauss-Seidel relaxation
- Output resolution
- Compiler optimization levels

The code includes several optimizations:
- Reusing arrays to minimize memory allocation
- Optimizing boundary checks
- Using compiler flags for performance (`/O2` or `-O2`)
- Limiting frame rate to prevent excessive CPU usage

## Extending the Codebase

The modular design allows for several extensions:

### Adding Interaction

The simulation could be extended to allow user interaction:
- Mouse input to add fluid or forces
- Interactive parameter adjustment
- Real-time visualization using a proper GUI library

### Visual Enhancements

Potential visual improvements:
- Color schemes for velocity visualization
- Particle-based rendering
- Streamlines or vector field visualization
- Higher resolution output

### Physical Enhancements

The simulation could be extended with:
- Obstacles and solid boundaries
- Multiple fluid types with different properties
- Temperature effects and buoyancy
- Surface tension

### Performance Improvements

For better performance:
- Multi-threading for parallel computation
- GPU acceleration using CUDA or OpenCL
- Adaptive grid resolution
- More efficient numerical methods

To implement these extensions, you would:
1. Modify the appropriate class (`FluidSimulator` for physics, `Renderer` for visuals)
2. Add new parameters and methods as needed
3. Update the main simulation loop to include new features
4. Adjust the build system if additional dependencies are required