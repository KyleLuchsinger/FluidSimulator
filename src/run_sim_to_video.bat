@echo off
setlocal

REM Default values
set FRAME_COUNT=300
set FPS=30
set OUTPUT_NAME=fluid_simulation

REM Parse command line arguments
if not "%1"=="" (
    set FRAME_COUNT=%1
)

if not "%2"=="" (
    set FPS=%2
)

if not "%3"=="" (
    set OUTPUT_NAME=%3
)

echo ===== Fluid Simulation to Video =====
echo Frames to generate: %FRAME_COUNT%
echo Video framerate: %FPS% fps
echo Output filename: %OUTPUT_NAME%.mp4
echo =====================================
echo.

REM Build and run simulation with specified frame count
echo Step 1: Building and running fluid simulation...
call build.bat run %FRAME_COUNT%

if %ERRORLEVEL% NEQ 0 (
    echo Error: Simulation failed to run correctly.
    exit /b 1
)

REM Convert frames to video
echo.
echo Step 2: Converting frames to video...
call convert_to_video.bat %FPS% %OUTPUT_NAME%

if %ERRORLEVEL% NEQ 0 (
    echo Error: Video conversion failed.
    exit /b 1
)

echo.
echo Process complete! Video saved as %OUTPUT_NAME%.mp4
echo.