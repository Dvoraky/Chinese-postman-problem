#ifndef __PCC_H
#define __PCC_H

#include <bits/stdc++.h>
#include <chrono>
#include <omp.h>
#include <thread>

#define ORIGEM 0

using namespace std;

typedef struct Aresta 
{
    int destino;
    int custo;
    bool marcado = false;
} Aresta;

class Grafo
{
    private:
        // Grafo
        vector<vector<Aresta>> grafo;
        int tamanho;

        // FLoyd-Warshall
        vector<vector<int>> dist;   // Matriz de distâncias do Floyd-Warshall
        vector<vector<int>> next;   // Matriz de predecessores para reconstruir caminhos

        // GLS
        double lambda; // Parâmetro de penalizaçã
        vector<vector<int>> penalidades; // Matriz de penalidades
        
        // Colônia de Formigas
        vector<vector<double>> feromonios; // Matriz de feromônios
        double taxa_evaporacao = 0.1;      // Taxa de evaporação (ρ)
        double alpha = 1.0;                // Peso do feromônio
        double beta = 2.0;                 // Peso da heurística (1/custo)

    public:
        // Construtores
        Grafo(string nomeArquivo);
        Grafo(const Grafo& outro);

        // Funções Auxiliares
        void adicionar_aresta_direcionadas(int origem, int destino, int custo);
        void adicionar_arestas_bidirecionadas(int origem, int destino, int custo);
        void imprimir_grafo();
        void imprimir_grafo_com_bool();
        void floydWarshall();
        int get_tamanho();
        void caminho_percorrido(vector<int> caminho);
        void caminho_percorrido_2(vector<int> caminho);
        int grau(int vertice);
        vector<int> restaurar_caminho(int origem, int destino); 

        // Carteiro Chinês
        void carteiro_chines(int vertice_atual, int &custo, vector <int>& caminho, vector <int> & graus);

        // Busca Local
        void reset_arestas();
        void ordenar_arestas();
        void BuscaLocalOpenMP(int &custo_atual, vector<int> &caminho_atual); 

        // GLS
        void BuscaLocalOpenMP_GLS(int &custo_atual, vector<int> &caminho_atual);
        void GuidedLocalSearch(int &custo_atual, vector<int> &caminho_atual, int tempo_maximo_minuto = 1); 
        int calcularSomaPenalidades(const vector<int>& caminho);
        int calcularCustoSemPenalidade(const vector<int>& caminho);
        void atualizarPenalidades(const vector<int>& caminho);
        void resetarPenalidades();  
        
        // Colônica de Formigas
        void ACO(int &custo, vector<int> &caminho, int origem, int max_iteracoes, int num_formigas);
};

#endif