#pragma once

#include <cstdint>
#include <vector>

#include "FluidSimulator.h"

struct Color {
    uint8_t r, g, b;

    Color() : r(0), g(0), b(0) {}
    Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b) {}
};

class Renderer {
   public:
    Renderer(int width, int height, int gridSize);
    ~Renderer();

    void clear();
    void renderFluid(const FluidSimulator& simulator);
    void display();
    bool isRunning() const;

   private:
    int width;
    int height;
    int gridSize;
    std::vector<Color> frameBuffer;

    FILE* outputFile;
    int frameCount;

    void setPixel(int x, int y, const Color& color);
    Color densityToColor(float density) const;
    void writePPM(const char* filename);
};