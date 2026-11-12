CXX = g++
CXXFLAGS = -O3 -fopenmp -std=c++17 -Wall -Wextra

TARGET = check_openmp.exe
SRC = check_openmp.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Check OpenMP compiler, runtime functions, and pragma directives
check: $(TARGET)
	./$(TARGET)

# Test ALL algorithms and code in the entire project
test-all:
	$(MAKE) -f Makefile.test_all test-all

clean:
	rm -f $(TARGET) *.o
	$(MAKE) -f Makefile.test_all clean

.PHONY: all check test-all clean
