# Rohan Krishnaswamy
# July 11, 2020

CXX = g++
CXXFLAGS = -std=c++17 -Wall -g

BIN = bin
SRC = src
LIBRARIES = -lpthread
INCLUDE = include
EXECUTABLE = main

all: $(BIN)/$(EXECUTABLE)

run: all
	clear
	@echo "🚀 Executing..."
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	@echo "👽 Compiling..."
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) $^ -o $@ $(LIBRARIES)

clean:
	@echo "🧹 Cleaning..."
	-rm $(BIN)/*