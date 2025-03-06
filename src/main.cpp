#include <chrono>
#include <iostream>
#include <thread>

#include "FluidSimulator.h"
#include "Renderer.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int GRID_SIZE = 100;
const float TIME_STEP = 0.1f;
const float DIFFUSION = 0.0001f;
const float VISCOSITY = 0.0001f;

int main() {
    FluidSimulator simulator(GRID_SIZE, TIME_STEP, DIFFUSION, VISCOSITY);
    Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT, GRID_SIZE);

    bool running = true;

    // Add some initial density and velocity
    simulator.addDensity(GRID_SIZE / 2, GRID_SIZE / 2, 100.0f);
    simulator.addVelocity(GRID_SIZE / 2, GRID_SIZE / 2, 5.0f, 0.0f);

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