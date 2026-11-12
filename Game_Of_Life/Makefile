CXX = g++
CXXFLAGS = -O3 -fopenmp -std=c++17 -Wall -Wextra

TARGET = game_of_life.exe
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC) game_of_life.hpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET) 2048 2048 50 8

clean:
	rm -f $(TARGET) *.o

