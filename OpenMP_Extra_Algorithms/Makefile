CXX=g++
CXXFLAGS=-std=c++17 -O2 -Wall -Wextra -fopenmp
SOURCES=$(wildcard *.cpp)
BINS=$(SOURCES:.cpp=)
all: $(BINS)
%: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@
test: all
	@set -e; for x in $(BINS); do echo "=== $$x ==="; ./$$x 4; done
clean:
	rm -f $(BINS) *.exe
.PHONY: all test clean
