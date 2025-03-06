# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2

# Source files and target
SRC = main.cpp FluidSimulator.cpp Renderer.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = fluid_sim

# Default target
all: $(TARGET)

# Link the target
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create directories for output
init:
	mkdir -p frames

# Clean up
clean:
	rm -f $(OBJ) $(TARGET)
	rm -rf frames/*.ppm

# Run the simulation
run: all init
	./$(TARGET)

# Dependencies
main.o: main.cpp FluidSimulator.h Renderer.h
FluidSimulator.o: FluidSimulator.cpp FluidSimulator.h
Renderer.o: Renderer.cpp Renderer.h FluidSimulator.h

.PHONY: all init clean run