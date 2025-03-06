@echo off
mkdir build 2>nul
mkdir frames 2>nul

if "%1"=="clean" (
    echo Cleaning...
    del /q build\*.* 2>nul
    del /q frames\*.* 2>nul
    del /q fluid_sim.exe 2>nul
    goto end
)

echo Compiling...
cl.exe /nologo /EHsc /W4 /std:c++17 /O2 /MD /I. /Fo:build/ /Fe:fluid_sim.exe main.cpp FluidSimulator.cpp Renderer.cpp /link /SUBSYSTEM:CONSOLE

if "%1"=="run" (
    echo Running...
    fluid_sim.exe
)

:end
echo Done.