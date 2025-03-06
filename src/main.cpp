#include <chrono>
#include <iostream>
#include <thread>

#include "FluidSimulator.h"
#include "Renderer.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int GRID_SIZE = 100;
const float TIME_STEP = 0.1f;
const float DIFFUSION = 0.00005f;
const float VISCOSITY = 0.0001f;

int main() {
    FluidSimulator simulator(GRID_SIZE, TIME_STEP, DIFFUSION, VISCOSITY);
    Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT, GRID_SIZE);

    bool running = true;

    for (int i = -5; i <= 5; i++) {
        for (int j = -5; j <= 5; j++) {
            float distance = sqrt(i * i + j * j);
            if (distance <= 5) {
                simulator.addDensity(GRID_SIZE / 2 + i, GRID_SIZE / 2 + j,
                                     200.0f * (1.0f - distance / 5.0f));
            }
        }
    }

    simulator.addVelocity(GRID_SIZE / 2, GRID_SIZE / 2, 20.0f, 0.0f);
    simulator.addVelocity(GRID_SIZE / 2 - 10, GRID_SIZE / 2 + 10, 10.0f, -10.0f);
    simulator.addVelocity(GRID_SIZE / 2 + 10, GRID_SIZE / 2 + 10, -10.0f, -10.0f);

    // FPS calculation
    auto lastTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;

    while (running) {
        simulator.step();

        renderer.clear();
        renderer.renderFluid(simulator);
        renderer.display();

        // Calculate and display FPS
        frameCount++;
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime =
            std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count();

        if (elapsedTime >= 1) {
            std::cout << "FPS: " << frameCount / elapsedTime << std::endl;
            frameCount = 0;
            lastTime = currentTime;
        }

        running = renderer.isRunning();

        // Cap frame rate
        std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
    }

    return 0;
}