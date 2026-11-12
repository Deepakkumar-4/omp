CXX = g++
CXXFLAGS = -O3 -fopenmp -std=c++17 -Wall -Wextra

TARGET = graph_benchmark.exe
SRC = main.cpp
HEADERS = csr_graph.hpp bfs.hpp pagerank.hpp sssp.hpp connected_components.hpp

all: $(TARGET)

$(TARGET): $(SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET) 50000 20.0 8

clean:
	rm -f $(TARGET) *.o

