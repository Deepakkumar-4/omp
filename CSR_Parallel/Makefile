CXX = g++
CXXFLAGS = -O3 -fopenmp -std=c++17 -Wall -Wextra

TARGET = csr_benchmark.exe
SRC = main.cpp

all: $(TARGET)

$(TARGET): $(SRC) parallel_csr.hpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET) 100000 2000000 8

clean:
	rm -f $(TARGET) *.o

