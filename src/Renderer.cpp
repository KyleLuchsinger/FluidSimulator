#include "Renderer.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <Windows.h>
#include <direct.h>  // For _mkdir
#define MKDIR(dir) _mkdir(dir)
#else
#include <sys/ioctl.h>
#include <sys/stat.h>  // For mkdir
#include <termios.h>
#include <unistd.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif

Renderer::Renderer(int width, int height, int gridSize)
    : width(width), height(height), gridSize(gridSize), frameCount(0) {
    frameBuffer.resize(width * height);
    clear();

    MKDIR("frames");

    std::cout << "Renderer initialized with resolution " << width << "x" << height << std::endl;
    std::cout << "Frames will be saved to the 'frames' directory as PPM files" << std::endl;
    std::cout << "Press Ctrl+C to exit" << std::endl;
}

Renderer::~Renderer() {}

void Renderer::clear() { std::fill(frameBuffer.begin(), frameBuffer.end(), Color(0, 0, 0)); }

void Renderer::renderFluid(const FluidSimulator& simulator) {
    const float* density = simulator.getDensity();
    const float* velocityX = simulator.getVelocityX();
    const float* velocityY = simulator.getVelocityY();
    const int simSize = simulator.getSize();

    float scaleX = static_cast<float>(width) / simSize;
    float scaleY = static_cast<float>(height) / simSize;

    for (int y = 0; y < simSize; y++) {
        for (int x = 0; x < simSize; x++) {
            float d = density[x + y * simSize];

            if (d < 0.001f) continue;  // Lower threshold for better visibility

            Color color = densityToColor(d);

            int screenX = static_cast<int>(x * scaleX);
            int screenY = static_cast<int>(y * scaleY);
            // Use (std::max)(1, ...) to avoid macro max issues with MSVC
            int screenWidth = (std::max)(1, static_cast<int>(scaleX));
            int screenHeight = (std::max)(1, static_cast<int>(scaleY));

            for (int dy = 0; dy < screenHeight; dy++) {
                for (int dx = 0; dx < screenWidth; dx++) {
                    int px = screenX + dx;
                    int py = screenY + dy;

                    if (px >= 0 && px < width && py >= 0 && py < height) {
                        setPixel(px, py, color);
                    }
                }
            }
        }
    }
}

void Renderer::display() {
    char filename[100];
    sprintf(filename, "frames/frame_%04d.ppm", frameCount++);

    writePPM(filename);

    std::cout << "Frame " << frameCount << " rendered to " << filename << std::endl;
}

bool Renderer::isRunning() const { return true; }

void Renderer::setPixel(int x, int y, const Color& color) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }

    frameBuffer[y * width + x] = color;
}

Color Renderer::densityToColor(float density) const {
    // Map density to a color
    density = (std::min)(1.0f, (std::max)(0.0f, density / 50.0f));  // Increased brightness

    // HSV to RGB conversion for a nice color gradient
    float h = (1.0f - density) * 240.0f;  // Hue (0-360) - from red to blue
    float s = 1.0f;                       // Saturation
    float v = density;                    // Value

    // HSV to RGB conversion
    float c = v * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    float r, g, b;

    if (h < 60.0f) {
        r = c;
        g = x;
        b = 0;
    } else if (h < 120.0f) {
        r = x;
        g = c;
        b = 0;
    } else if (h < 180.0f) {
        r = 0;
        g = c;
        b = x;
    } else if (h < 240.0f) {
        r = 0;
        g = x;
        b = c;
    } else if (h < 300.0f) {
        r = x;
        g = 0;
        b = c;
    } else {
        r = c;
        g = 0;
        b = x;
    }

    return Color(static_cast<uint8_t>((r + m) * 255), static_cast<uint8_t>((g + m) * 255),
                 static_cast<uint8_t>((b + m) * 255));
}

void Renderer::writePPM(const char* filename) {
    std::ofstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing" << std::endl;
        return;
    }

    // Write the PPM header
    file << "P6\n" << width << " " << height << "\n255\n";

    // Write the pixel data
    for (const auto& pixel : frameBuffer) {
        file.write(reinterpret_cast<const char*>(&pixel.r), 1);
        file.write(reinterpret_cast<const char*>(&pixel.g), 1);
        file.write(reinterpret_cast<const char*>(&pixel.b), 1);
    }

    file.close();
}