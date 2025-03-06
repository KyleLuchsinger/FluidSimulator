#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "FluidSimulator.h"
#include "Renderer.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int GRID_SIZE = 100;
const float TIME_STEP = 0.1f;
const float DIFFUSION = 0.00005f;
const float VISCOSITY = 0.0001f;
const int DEFAULT_MAX_FRAMES = 300;  // Default number of frames to generate

int main(int argc, char* argv[]) {
    // Parse command line arguments for max frames
    int maxFrames = DEFAULT_MAX_FRAMES;
    if (argc > 1) {
        try {
            maxFrames = std::stoi(argv[1]);
            if (maxFrames <= 0) {
                std::cout << "Invalid frame count. Using default: " << DEFAULT_MAX_FRAMES
                          << std::endl;
                maxFrames = DEFAULT_MAX_FRAMES;
            }
        } catch (const std::exception&) {
            std::cout << "Invalid argument. Using default frame count: " << DEFAULT_MAX_FRAMES
                      << std::endl;
        }
    }

    std::cout << "Simulation will generate " << maxFrames << " frames" << std::endl;

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
    int fpsCount = 0;

    while (running && frameCount < maxFrames) {
        simulator.step();

        renderer.clear();
        renderer.renderFluid(simulator);
        renderer.display();

        // Update counters
        frameCount++;
        fpsCount++;

        // Calculate and display FPS
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime =
            std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count();

        if (elapsedTime >= 1) {
            std::cout << "FPS: " << fpsCount / elapsedTime << " (Frame " << frameCount << "/"
                      << maxFrames << ")" << std::endl;
            fpsCount = 0;
            lastTime = currentTime;
        }

        running = renderer.isRunning();

        // Cap frame rate
        std::this_thread::sleep_for(std::chrono::milliseconds(16));  // ~60 FPS
    }

    std::cout << "Simulation complete. Generated " << frameCount << " frames." << std::endl;
    return 0;
}