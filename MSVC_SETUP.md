# Setting up MSVC for the Fluid Simulation Project

This guide will help you set up Microsoft Visual C++ (MSVC) compiler for building the fluid simulation project.

## Prerequisites

1. **Visual Studio** - You need one of the following:
   - Visual Studio 2022 (Community, Professional, or Enterprise)
   - Visual Studio 2019 (Community, Professional, or Enterprise)
   - Visual Studio Build Tools (if you don't want the full IDE)

2. **Windows SDK** - This is typically installed with Visual Studio

## Using the Simplified Build Script

A simplified build script (`build.bat`) has been provided for maximum compatibility. Here's how to use it:

1. Open "Developer Command Prompt for VS 2022" (or your appropriate VS version) from the Start menu
2. Navigate to the project directory
3. Run the build commands:
   ```
   build.bat       # Build only
   build.bat run   # Build and run
   build.bat clean # Clean the project
   ```

## Manual Build Commands

If you're having issues with the batch file, you can use these commands directly in the Developer Command Prompt:

```
mkdir build
mkdir frames
cl.exe /nologo /EHsc /W4 /std:c++17 /O2 /MD /I. /Fo:build/ /Fe:fluid_sim.exe main.cpp FluidSimulator.cpp Renderer.cpp /link /SUBSYSTEM:CONSOLE
```

To run the simulation:
```
fluid_sim.exe
```

## Troubleshooting

### "cl.exe not found"

This error means the MSVC compiler is not in your PATH. Make sure you're using the Developer Command Prompt for Visual Studio, not a regular command prompt.

### "not was unexpected at this time"

If you encounter this error with the batch file:
1. Try the manual build commands listed above
2. Make sure the batch file is saved with Windows line endings (CRLF)
3. Try creating the batch file by typing it manually rather than copying/pasting

### Missing Windows SDK

If you encounter errors about missing Windows SDK headers, make sure you have the Windows SDK installed. You can install it through the Visual Studio Installer by modifying your Visual Studio installation and selecting the "Windows SDK" component.

### Build fails with link errors

Make sure you're compiling with the `/MD` flag to link against the multithreaded DLL runtime.

## Additional Notes

- The project uses C++17 features, so make sure your MSVC version supports C++17 (Visual Studio 2017 version 15.7 or later)
- The build script creates a `build` directory for intermediate files and a `frames` directory for output
- Frames are saved as PPM files which are a simple image format; you may want to convert them to a more common format using an image processing tool