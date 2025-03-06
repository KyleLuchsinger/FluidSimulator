@echo off
echo Converting frames to video...

if not exist frames (
    echo Error: frames directory not found!
    exit /b 1
)

set COUNT=0
for %%f in (frames\*.ppm) do set /a COUNT+=1

if %COUNT% EQU 0 (
    echo No frames found in the frames directory!
    exit /b 1
)

echo Found %COUNT% frames to process

if "%1"=="" (
    set FPS=30
) else (
    set FPS=%1
)

echo Using framerate: %FPS% fps

ffmpeg -y -framerate %FPS% -i frames/frame_%%04d.ppm -c:v libx264 -pix_fmt yuv420p -crf 23 fluid_simulation.mp4

if %ERRORLEVEL% NEQ 0 (
    echo Error: FFmpeg failed to convert frames to video.
    echo Make sure FFmpeg is installed and in your PATH.
    exit /b 1
)

echo Video created successfully: fluid_simulation.mp4