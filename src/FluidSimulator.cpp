#include "FluidSimulator.h"

#include <algorithm>
#include <cstring>

FluidSimulator::FluidSimulator(int size, float dt, float diffusion, float viscosity)
    : size(size), dt(dt), diffusion(diffusion), viscosity(viscosity) {
    int arraySize = size * size;

    density = new float[arraySize]();
    densityPrev = new float[arraySize]();

    velocityX = new float[arraySize]();
    velocityY = new float[arraySize]();
    velocityPrevX = new float[arraySize]();
    velocityPrevY = new float[arraySize]();
}

FluidSimulator::~FluidSimulator() {
    delete[] density;
    delete[] densityPrev;
    delete[] velocityX;
    delete[] velocityY;
    delete[] velocityPrevX;
    delete[] velocityPrevY;
}

void FluidSimulator::step() {
    // Velocity step
    diffuse(1, velocityPrevX, velocityX, viscosity, dt);
    diffuse(2, velocityPrevY, velocityY, viscosity, dt);

    project(velocityPrevX, velocityPrevY, velocityX, velocityY);

    advect(1, velocityX, velocityPrevX, velocityPrevX, velocityPrevY, dt);
    advect(2, velocityY, velocityPrevY, velocityPrevX, velocityPrevY, dt);

    project(velocityX, velocityY, velocityPrevX, velocityPrevY);

    // Density step
    diffuse(0, densityPrev, density, diffusion, dt);
    advect(0, density, densityPrev, velocityX, velocityY, dt);
}

void FluidSimulator::addDensity(int x, int y, float amount) { density[IX(x, y)] += amount; }

void FluidSimulator::addVelocity(int x, int y, float amountX, float amountY) {
    velocityX[IX(x, y)] += amountX;
    velocityY[IX(x, y)] += amountY;
}

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

void FluidSimulator::project(float* velocX, float* velocY, float* p, float* div) {
    for (int j = 1; j < size - 1; j++) {
        for (int i = 1; i < size - 1; i++) {
            div[IX(i, j)] = -0.5f *
                            (velocX[IX(i + 1, j)] - velocX[IX(i - 1, j)] + velocY[IX(i, j + 1)] -
                             velocY[IX(i, j - 1)]) /
                            size;
            p[IX(i, j)] = 0;
        }
    }

    setBoundary(0, div);
    setBoundary(0, p);

    // Gauss-Seidel relaxation
    for (int k = 0; k < 20; k++) {
        for (int j = 1; j < size - 1; j++) {
            for (int i = 1; i < size - 1; i++) {
                p[IX(i, j)] = (div[IX(i, j)] + (p[IX(i + 1, j)] + p[IX(i - 1, j)] +
                                                p[IX(i, j + 1)] + p[IX(i, j - 1)])) /
                              4;
            }
        }
        setBoundary(0, p);
    }

    for (int j = 1; j < size - 1; j++) {
        for (int i = 1; i < size - 1; i++) {
            velocX[IX(i, j)] -= 0.5f * (p[IX(i + 1, j)] - p[IX(i - 1, j)]) * size;
            velocY[IX(i, j)] -= 0.5f * (p[IX(i, j + 1)] - p[IX(i, j - 1)]) * size;
        }
    }

    setBoundary(1, velocX);
    setBoundary(2, velocY);
}

void FluidSimulator::advect(int b, float* d, float* d0, float* velocX, float* velocY, float dt) {
    float i0, i1, j0, j1;
    float s0, s1, t0, t1;
    float x, y, dt0;

    dt0 = dt * (size - 2);

    for (int j = 1; j < size - 1; j++) {
        for (int i = 1; i < size - 1; i++) {
            // Backtrack
            x = i - dt0 * velocX[IX(i, j)];
            y = j - dt0 * velocY[IX(i, j)];

            // Clamp to boundaries
            x = std::max(0.5f, std::min(size - 1.5f, x));
            y = std::max(0.5f, std::min(size - 1.5f, y));

            // Integer positions
            i0 = static_cast<int>(x);
            j0 = static_cast<int>(y);
            i1 = i0 + 1;
            j1 = j0 + 1;

            // Fractional positions
            s1 = x - i0;
            s0 = 1 - s1;
            t1 = y - j0;
            t0 = 1 - t1;

            // Bilinear interpolation
            d[IX(i, j)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
                          s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
        }
    }

    setBoundary(b, d);
}

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