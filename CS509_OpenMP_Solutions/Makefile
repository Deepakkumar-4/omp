CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -fopenmp

TARGETS = buddy_assignment_01_bfs buddy_assignment_01_dfs buddy_assignment_01_sssp buddy_assignment_02_betweenness buddy_assignment_02_components buddy_assignment_02_triangle_counting buddy_assignment_03_gradient_descent buddy_assignment_03_maxflow buddy_assignment_04_fastmap buddy_assignment_04_kmeans individual_assignment_01_blocked_gemm individual_assignment_01_gemm individual_assignment_02_bellman_ford individual_assignment_02_floyd_warshall individual_assignment_03_kruskal individual_assignment_03_prim individual_assignment_04_pagerank individual_assignment_04_vertex_coloring

all: $(TARGETS)

buddy_assignment_01_bfs: buddy/assignment_01/bfs.cpp common.hpp
	$(CXX) $(CXXFLAGS) buddy/assignment_01/bfs.cpp -o buddy_assignment_01_bfs

run_buddy_assignment_01_bfs: buddy_assignment_01_bfs
	./buddy_assignment_01_bfs 4

buddy_assignment_01_dfs: buddy/assignment_01/dfs.cpp common.hpp
	$(CXX) $(CXXFLAGS) buddy/assignment_01/dfs.cpp -o buddy_assignment_01_dfs

run_buddy_assignment_01_dfs: buddy_assignment_01_dfs
	./buddy_assignment_01_dfs 4

buddy_assignment_01_sssp: buddy/assignment_01/sssp.cpp common.hpp
	$(CXX) $(CXXFLAGS) buddy/assignment_01/sssp.cpp -o buddy_assignment_01_sssp

run_buddy_assignment_01_sssp: buddy_assignment_01_sssp
	./buddy_assignment_01_sssp 4

buddy_assignment_02_betweenness: buddy/assignment_02/betweenness.cpp common.hpp
	$(CXX) $(CXXFLAGS) buddy/assignment_02/betweenness.cpp -o buddy_assignment_02_betweenness

run_buddy_assignment_02_betweenness: buddy_assignment_02_betweenness
	./buddy_assignment_02_betweenness 4

buddy_assignment_02_components: buddy/assignment_02/components.cpp common.hpp
	$(CXX) $(CXXFLAGS) buddy/assignment_02/components.cpp -o buddy_assignment_02_components

run_buddy_assignment_02_components: buddy_assignment_02_components
	./buddy_assignment_02_components 4

buddy_assignment_02_triangle_counting: buddy/assignment_02/triangle_counting.cpp common.hpp
	$(CXX) $(CXXFLAGS) buddy/assignment_02/triangle_counting.cpp -o buddy_assignment_02_triangle_counting

run_buddy_assignment_02_triangle_counting: buddy_assignment_02_triangle_counting
	./buddy_assignment_02_triangle_counting 4

buddy_assignment_03_gradient_descent: buddy/assignment_03/gradient_descent.cpp common.hpp
	$(CXX) $(CXXFLAGS) buddy/assignment_03/gradient_descent.cpp -o buddy_assignment_03_gradient_descent

run_buddy_assignment_03_gradient_descent: buddy_assignment_03_gradient_descent
	./buddy_assignment_03_gradient_descent 4

buddy_assignment_03_maxflow: buddy/assignment_03/maxflow.cpp common.hpp
	$(CXX) $(CXXFLAGS) buddy/assignment_03/maxflow.cpp -o buddy_assignment_03_maxflow

run_buddy_assignment_03_maxflow: buddy_assignment_03_maxflow
	./buddy_assignment_03_maxflow 4

buddy_assignment_04_fastmap: buddy/assignment_04/fastmap.cpp common.hpp
	$(CXX) $(CXXFLAGS) buddy/assignment_04/fastmap.cpp -o buddy_assignment_04_fastmap

run_buddy_assignment_04_fastmap: buddy_assignment_04_fastmap
	./buddy_assignment_04_fastmap 4

buddy_assignment_04_kmeans: buddy/assignment_04/kmeans.cpp common.hpp
	$(CXX) $(CXXFLAGS) buddy/assignment_04/kmeans.cpp -o buddy_assignment_04_kmeans

run_buddy_assignment_04_kmeans: buddy_assignment_04_kmeans
	./buddy_assignment_04_kmeans 4

individual_assignment_01_blocked_gemm: individual/assignment_01/blocked_gemm.cpp common.hpp
	$(CXX) $(CXXFLAGS) individual/assignment_01/blocked_gemm.cpp -o individual_assignment_01_blocked_gemm

run_individual_assignment_01_blocked_gemm: individual_assignment_01_blocked_gemm
	./individual_assignment_01_blocked_gemm 4

individual_assignment_01_gemm: individual/assignment_01/gemm.cpp common.hpp
	$(CXX) $(CXXFLAGS) individual/assignment_01/gemm.cpp -o individual_assignment_01_gemm

run_individual_assignment_01_gemm: individual_assignment_01_gemm
	./individual_assignment_01_gemm 4

individual_assignment_02_bellman_ford: individual/assignment_02/bellman_ford.cpp common.hpp
	$(CXX) $(CXXFLAGS) individual/assignment_02/bellman_ford.cpp -o individual_assignment_02_bellman_ford

run_individual_assignment_02_bellman_ford: individual_assignment_02_bellman_ford
	./individual_assignment_02_bellman_ford 4

individual_assignment_02_floyd_warshall: individual/assignment_02/floyd_warshall.cpp common.hpp
	$(CXX) $(CXXFLAGS) individual/assignment_02/floyd_warshall.cpp -o individual_assignment_02_floyd_warshall

run_individual_assignment_02_floyd_warshall: individual_assignment_02_floyd_warshall
	./individual_assignment_02_floyd_warshall 4

individual_assignment_03_kruskal: individual/assignment_03/kruskal.cpp common.hpp
	$(CXX) $(CXXFLAGS) individual/assignment_03/kruskal.cpp -o individual_assignment_03_kruskal

run_individual_assignment_03_kruskal: individual_assignment_03_kruskal
	./individual_assignment_03_kruskal 4

individual_assignment_03_prim: individual/assignment_03/prim.cpp common.hpp
	$(CXX) $(CXXFLAGS) individual/assignment_03/prim.cpp -o individual_assignment_03_prim

run_individual_assignment_03_prim: individual_assignment_03_prim
	./individual_assignment_03_prim 4

individual_assignment_04_pagerank: individual/assignment_04/pagerank.cpp common.hpp
	$(CXX) $(CXXFLAGS) individual/assignment_04/pagerank.cpp -o individual_assignment_04_pagerank

run_individual_assignment_04_pagerank: individual_assignment_04_pagerank
	./individual_assignment_04_pagerank 4

individual_assignment_04_vertex_coloring: individual/assignment_04/vertex_coloring.cpp common.hpp
	$(CXX) $(CXXFLAGS) individual/assignment_04/vertex_coloring.cpp -o individual_assignment_04_vertex_coloring

run_individual_assignment_04_vertex_coloring: individual_assignment_04_vertex_coloring
	./individual_assignment_04_vertex_coloring 4

test: all
	./buddy_assignment_01_bfs 4
	./buddy_assignment_01_dfs 4
	./buddy_assignment_01_sssp 4
	./buddy_assignment_02_betweenness 4
	./buddy_assignment_02_components 4
	./buddy_assignment_02_triangle_counting 4
	./buddy_assignment_03_gradient_descent 4
	./buddy_assignment_03_maxflow 4
	./buddy_assignment_04_fastmap 4
	./buddy_assignment_04_kmeans 4
	./individual_assignment_01_blocked_gemm 4
	./individual_assignment_01_gemm 4
	./individual_assignment_02_bellman_ford 4
	./individual_assignment_02_floyd_warshall 4
	./individual_assignment_03_kruskal 4
	./individual_assignment_03_prim 4
	./individual_assignment_04_pagerank 4
	./individual_assignment_04_vertex_coloring 4

clean:
	rm -f $(TARGETS) *.exe

.PHONY: all test clean run_buddy_assignment_01_bfs run_buddy_assignment_01_dfs run_buddy_assignment_01_sssp run_buddy_assignment_02_betweenness run_buddy_assignment_02_components run_buddy_assignment_02_triangle_counting run_buddy_assignment_03_gradient_descent run_buddy_assignment_03_maxflow run_buddy_assignment_04_fastmap run_buddy_assignment_04_kmeans run_individual_assignment_01_blocked_gemm run_individual_assignment_01_gemm run_individual_assignment_02_bellman_ford run_individual_assignment_02_floyd_warshall run_individual_assignment_03_kruskal run_individual_assignment_03_prim run_individual_assignment_04_pagerank run_individual_assignment_04_vertex_coloring
