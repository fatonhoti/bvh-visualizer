# Compiler and flags
CXX = g++
CXXFLAGS = -O3 -std=c++20 -Wall -pedantic -I ./include -I ./external -fopenmp
LDFLAGS = -lglfw -lGL -fopenmp

# Source files
SRC_DIR = ./src
EXT_SRC_DIR = ./external
SRCS = $(wildcard $(SRC_DIR)/*.cpp) $(EXT_SRC_DIR)/glad/glad.c $(wildcard $(EXT_SRC_DIR)/imgui/*.cpp)

# Object files
OBJS = $(SRCS:.cpp=.o)

# Output executable
TARGET = main.out

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Run the executable
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
