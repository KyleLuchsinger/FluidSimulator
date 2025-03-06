#pragma once

class FluidSimulator {
   public:
    FluidSimulator(int size, float dt, float diffusion, float viscosity);
    ~FluidSimulator();

    void step();
    void addDensity(int x, int y, float amount);
    void addVelocity(int x, int y, float amountX, float amountY);

    float* getDensity() const { return density; }
    float* getVelocityX() const { return velocityX; }
    float* getVelocityY() const { return velocityY; }
    int getSize() const { return size; }

   private:
    int size;         // Grid size
    float dt;         // Time step
    float diffusion;  // Diffusion rate
    float viscosity;  // Viscosity

    float* density;      // Density field
    float* densityPrev;  // Previous density field

    float* velocityX;      // Velocity field (x component)
    float* velocityY;      // Velocity field (y component)
    float* velocityPrevX;  // Previous velocity field (x component)
    float* velocityPrevY;  // Previous velocity field (y component)

    void diffuse(int b, float* x, float* x0, float diff, float dt);
    void project(float* velocX, float* velocY, float* p, float* div);
    void advect(int b, float* d, float* d0, float* velocX, float* velocY, float dt);
    void setBoundary(int b, float* x);

    int IX(int x, int y) const { return x + y * size; }
};