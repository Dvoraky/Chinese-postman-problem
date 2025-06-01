# Chinese-postman-problem

Este repositório apresenta soluções para o problema do carteiro chinês em grafos mistos, sendo elas um gulosa, uma busca local, e duas implementações de metaheurísticas, sendo elas a GLS (Guided Local Search) e a ACO (Ant Colony Optimization), assim como uma visualização gráfica do grafo e seu caminho percorrido. A heuristica gulosa utiliza do vizinho mais próximo e a busca local faz uma troca na ordem das arestas a serem percorridas. É utilizado o algoritmo de Floyd-Warshall para voltar à origem pelo caminho mínimo ou para perccorer o caminho mais curto para o vértice com arestas ainda não percorridas. E também foi utilizado OpenMP para paralelizar o programa.

This repository presents solutions to the Chinese Postman problem in mixed graphs, namely a greedy heuristic, a local search, and two metaheuristic implementations, namely GLS (Guided Local Search) and ACO (Ant Colony Optimization), as well as a graphical visualization of the graph and its path. The greedy heuristic uses the nearest neighbour and the local search makes a change in the order of the edges to be traversed. The Floyd-Warshall algorithm is used to return to the origin by the shortest path or to take the shortest path to the vertex with edges not yet traversed. OpenMP was also used to parallelize the program.

Windows:

g++ main.cpp PCC.cpp -fopenmp -O3 -Isrc/include -o main -Lsrc/lib -lsfml-graphics -lsfml-window -lsfml-system; & ./main.exe

Linux:

g++ main.cpp PCC.cpp -fopenmp -O3 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network -lsfml-audio && ./a.out




