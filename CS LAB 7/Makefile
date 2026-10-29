
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -fopenmp

TARGETS = task1 task2 task3 task4 task5

all: $(TARGETS)

task1: task1.cpp common.hpp
	$(CXX) $(CXXFLAGS) task1.cpp -o task1

task2: task2.cpp common.hpp
	$(CXX) $(CXXFLAGS) task2.cpp -o task2

task3: task3.cpp common.hpp
	$(CXX) $(CXXFLAGS) task3.cpp -o task3

task4: task4.cpp common.hpp
	$(CXX) $(CXXFLAGS) task4.cpp -o task4

task5: task5.cpp common.hpp
	$(CXX) $(CXXFLAGS) task5.cpp -o task5

run1: task1
	./task1 10000000 1
	./task1 10000000 2
	./task1 10000000 4
	./task1 10000000 8

run2: task2
	./task2 10000000 1
	./task2 10000000 2
	./task2 10000000 4
	./task2 10000000 8

run3: task3
	./task3 10000000 1
	./task3 10000000 2
	./task3 10000000 4
	./task3 10000000 8

run4: task4
	./task4 8 1
	./task4 8 2
	./task4 8 4
	./task4 8 8
	./task4 4096 1
	./task4 4096 2
	./task4 4096 4
	./task4 4096 8

run5: task5
	./task5 256 1
	./task5 256 2
	./task5 256 4
	./task5 256 8
	./task5 512 1
	./task5 512 2
	./task5 512 4
	./task5 512 8
	./task5 1024 1
	./task5 1024 2
	./task5 1024 4
	./task5 1024 8

clean:
	rm -f $(TARGETS) *.o

rebuild: clean all

.PHONY: all clean rebuild run1 run2 run3 run4 run5
