CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -fopenmp

SOURCES = searching/linear_search.cpp searching/binary_search_queries.cpp arrays/reduction_sum.cpp sorting/odd_even_sort.cpp sorting/merge_sort.cpp sorting/quick_sort.cpp recursion/fibonacci.cpp matrices/matrix_multiplication.cpp
TARGETS = $(SOURCES:.cpp=)

all: $(TARGETS)

%: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

test: all
	./searching/linear_search 10000 4
	./searching/binary_search_queries 10000 4
	./arrays/reduction_sum 10000 4
	./sorting/odd_even_sort 200 4
	./sorting/merge_sort 10000 4
	./sorting/quick_sort 10000 4
	./recursion/fibonacci 22 4
	./matrices/matrix_multiplication 64 4

clean:
	rm -f $(TARGETS) $(TARGETS:%=%.exe)

.PHONY: all test clean
