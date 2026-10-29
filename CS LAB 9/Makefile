CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -fopenmp
TARGETS = task1 task2 task3

all: $(TARGETS)

task1: task1.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
task2: task2.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
task3: task3.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

run1: task1
	./task1 100000 1
	./task1 100000 2
	./task1 100000 4
	./task1 100000 8
	./task1 1000000 1
	./task1 1000000 2
	./task1 1000000 4
	./task1 1000000 8
	./task1 10000000 1
	./task1 10000000 2
	./task1 10000000 4
	./task1 10000000 8

run2: task2
	./task2 100000 1
	./task2 100000 2
	./task2 100000 4
	./task2 100000 8
	./task2 1000000 1
	./task2 1000000 2
	./task2 1000000 4
	./task2 1000000 8
	./task2 10000000 1
	./task2 10000000 2
	./task2 10000000 4
	./task2 10000000 8

run3: task3
	./task3 100000 1
	./task3 100000 2
	./task3 100000 4
	./task3 100000 8
	./task3 1000000 1
	./task3 1000000 2
	./task3 1000000 4
	./task3 1000000 8
	./task3 10000000 1
	./task3 10000000 2
	./task3 10000000 4
	./task3 10000000 8
	./task3 100000000 1
	./task3 100000000 2
	./task3 100000000 4
	./task3 100000000 8

test: all
	./task1 10000 4
	./task2 1000 4
	./task3 10000 4

clean:
	rm -f $(TARGETS) *.o
rebuild: clean all
.PHONY: all run1 run2 run3 test clean rebuild
