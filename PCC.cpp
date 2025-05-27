#include <bits/stdc++.h>
#include <chrono>
#include <omp.h>
#include <thread>
#include "PCC.h"

using namespace std;
using namespace std::chrono;

const int INF = INT_MAX;


Grafo::Grafo(string nomeArquivo) : lambda(0.3)
{
    int numero_vertices, numero_arestas, vertice_1, vertice_2, custo;
    char tipo;
    
    ifstream arquivo(nomeArquivo);

    if(!arquivo.is_open())
    {
        cout << "Erro ao abrir o arquivo." << endl;
        exit(EXIT_FAILURE);
    }
    
    arquivo >> numero_vertices >> numero_arestas;

    this -> tamanho = numero_vertices;
    grafo.resize(numero_vertices);
    
    for(int j = 0; j < numero_arestas; j++)
    {
        arquivo >> vertice_1 >> tipo >> vertice_2 >> custo;

        if(tipo == '>') 
        {
            adicionar_aresta_direcionadas(vertice_1, vertice_2, custo);
        }
        else if(tipo =='-')
        {
            adicionar_arestas_bidirecionadas(vertice_1, vertice_2, custo);
        }
        else
        {
            cout << "O arquivo está mal formatado." << endl;
            exit(EXIT_FAILURE);
        }
    }
    
    arquivo.close();
    ordenar_arestas();
    floydWarshall();
    penalidades.resize(tamanho, vector<int>(tamanho, 0));
    feromonios.resize(tamanho, vector<double>(tamanho, 0.1));
}

Grafo::Grafo(const Grafo& outro) 
{
    tamanho = outro.tamanho;
    grafo = outro.grafo;
    dist = outro.dist;    // Copia a matriz de distâncias
    next = outro.next;    // Copia a matriz de predecessores
    penalidades = outro.penalidades; // Copia penalidades
    lambda = outro.lambda;
}
        

void Grafo::adicionar_aresta_direcionadas(int origem, int destino, int custo)
{
    grafo[origem].push_back({destino, custo});
}

void Grafo::adicionar_arestas_bidirecionadas(int origem, int destino, int custo)
{
    grafo[origem].push_back({destino, custo});
    grafo[destino].push_back({origem, custo});
}

void Grafo::imprimir_grafo()
{
    cout << "A impressão do grafo segue o padrão:" << endl;
    cout << "origem: (destino, custo) -> ..." << endl;
    for(int i = 0; i < grafo.size(); i++)
    {
        cout << i << ": ";
        for(int j = 0; j < grafo[i].size(); j++)
        {
            cout << "(" << grafo[i][j].destino << ", " << grafo[i][j].custo << ")" << (j == grafo[i].size() - 1 ? "" : " -> "); 
        }
        cout << endl;
    }
} 

void Grafo::imprimir_grafo_com_bool()
{
    cout << "A impressão do grafo segue o padrão:" << endl;
    cout << "origem: (destino, custo, foi marcado) -> ..." << endl;
    for(int i = 0; i < grafo.size(); i++)
    {
        cout << i << ": ";
        for(int j = 0; j < grafo[i].size(); j++)
        {
            cout << "(" << grafo[i][j].destino << ", " << grafo[i][j].custo << ", " << ((grafo[i][j].marcado) ? "True" : "False") << ")" << (j == grafo[i].size() - 1 ? "" : " -> "); 
        }
        cout << endl;
    }
}       

void Grafo::floydWarshall() 
{
    // Inicializa as matrizes dist e next
    dist.resize(tamanho, vector<int>(tamanho, INF));
    next.resize(tamanho, vector<int>(tamanho, -1));

    // Inicializa distâncias e predecessores
    for(int i = 0; i < tamanho; ++i) 
    {
        dist[i][i] = 0;
        for(auto &aresta : grafo[i]) 
        {
            dist[i][aresta.destino] = aresta.custo;
            next[i][aresta.destino] = aresta.destino;
        }
    }

    // Algoritmo Floyd-Warshall
    for(int k = 0; k < tamanho; ++k) 
    {
        for(int i = 0; i < tamanho; ++i) 
        {
            for(int j = 0; j < tamanho; ++j) 
            {
                if(dist[i][k] != INF && dist[k][j] != INF && dist[i][j] > dist[i][k] + dist[k][j]) 
                {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }
}

int Grafo::get_tamanho()
{
    return tamanho;
}

vector<int> Grafo::restaurar_caminho(int origem, int destino) 
{
    vector<int> caminho;
    if(next[origem][destino] == -1) return caminho;

    caminho.push_back(origem);
    while(origem != destino) 
    {
        origem = next[origem][destino];
        caminho.push_back(origem);
    }
    return caminho;
}

int Grafo::grau(int vertice)
{ 
    return grafo[vertice].size(); 
}

void Grafo::carteiro_chines(int vertice_atual, int &custo, vector <int>& caminho,vector <int> & graus)
{
    // Verifica se todas as arestas foram marcadas
    bool todas_marcadas = true;
    for(int vertice = 0 ; vertice < tamanho && todas_marcadas; vertice++)
    {
        for(auto &aresta : grafo[vertice]) 
        { 
            if(!aresta.marcado) 
            {
                todas_marcadas = false; 
                break;
            } 
        }
    }

    if(todas_marcadas) 
    { 
        if(vertice_atual != ORIGEM) 
        {
            auto path = restaurar_caminho(vertice_atual, ORIGEM);
            if(path.size() > 1) 
            { 
                caminho.insert(caminho.end(), path.begin() + 1, path.end()); 
                custo += dist[vertice_atual][ORIGEM]; 
            }
        } 
    }

    // Seleção gulosa da próxima aresta
    int melhor_custo = INF, proximo_vertice = -1;
    for(auto &aresta : grafo[vertice_atual])
    {
        if(!aresta.marcado)
        { 
            melhor_custo = aresta.custo; 
            proximo_vertice = aresta.destino; 
            break;
        }
    } 

    if(proximo_vertice != -1) 
    {
        // Marca as arestas nos dois sentidos
        for(auto &aresta : grafo[vertice_atual])
        { 
            if(aresta.destino == proximo_vertice && !aresta.marcado)
            {
                aresta.marcado = true;
                graus[vertice_atual]--; 
                break; 
            }
        }

        for(auto &aresta : grafo[proximo_vertice]) 
        {
            if(aresta.destino == vertice_atual && !aresta.marcado && melhor_custo == aresta.custo)
            { 
                aresta.marcado = true;
                graus[proximo_vertice]--;
                break; 
            }
        }

        caminho.push_back(proximo_vertice);
        custo += melhor_custo;
        carteiro_chines(proximo_vertice, custo, caminho, graus);
    } 
    else 
    {
        // Busca o próximo vértice via Floyd-Warshall
        int distancia_minima = INF, vertice_escolhido = -1;
        for(int vertice = 0; vertice < tamanho; vertice++) 
        {
            if(vertice != vertice_atual && graus[vertice] > 0 && dist[vertice_atual][vertice] < distancia_minima) 
            {
                distancia_minima = dist[vertice_atual][vertice]; 
                vertice_escolhido = vertice; 
            }
        }
        if(vertice_escolhido < 0) return;

        auto path = restaurar_caminho(vertice_atual, vertice_escolhido);
        if(path.size() > 1)
        { 
            caminho.insert(caminho.end(), path.begin() + 1, path.end()); 
            custo += distancia_minima; 
        }
        carteiro_chines(vertice_escolhido, custo, caminho, graus);
    } 

}

void Grafo::reset_arestas() 
{
    for(auto &adj : grafo) 
    {
        for(auto &aresta : adj) 
        {
            aresta.marcado = false;
        }
    }
} 

void Grafo::ordenar_arestas() 
{
    for(auto &adj : grafo) 
    {
        sort(adj.begin(), adj.end(), [](const Aresta& a, const Aresta& b) { return a.custo < b.custo;});
    }
}

void Grafo::BuscaLocalOpenMP(int &custo_atual, vector<int> &caminho_atual) 
{
    vector<int> graus_originais(tamanho);
    for(int i = 0; i < tamanho; ++i) 
    {
        graus_originais[i] = grau(i);
    }

    bool melhorou;
    melhorou = false;
    int melhor_custo_global = custo_atual;
    vector<int> melhor_caminho_global = caminho_atual;
    do 
    {
        melhorou = false;
        melhor_custo_global = custo_atual;
        melhor_caminho_global = caminho_atual;

        #pragma omp parallel
        {
            int melhor_custo_local = custo_atual;
            vector<int> melhor_caminho_local = caminho_atual;

            #pragma omp for schedule(dynamic)
            for(int v = 0; v < tamanho; v++) 
            {
                Grafo grafo_local = *this;
                grafo_local.reset_arestas(); // Reset crítico aqui

                for(int i = 0; i < (int)grafo_local.grafo[v].size() - 1; i++) 
                {
                    Grafo grafo_copia = grafo_local;
                    swap(grafo_copia.grafo[v][i], grafo_copia.grafo[v][i + 1]);

                    vector<int> novo_caminho;
                    novo_caminho.push_back(ORIGEM);
                    int novo_custo = 0;
                    vector<int> new_graus = graus_originais;

                    grafo_copia.reset_arestas(); // Reset adicional
                    grafo_copia.carteiro_chines(ORIGEM, novo_custo, novo_caminho, new_graus);

                    if(novo_custo < melhor_custo_local) 
                    {
                        melhor_custo_local = novo_custo;
                        melhor_caminho_local = novo_caminho;
                    }
                }
            }


            #pragma omp critical
            {
                if(melhor_custo_local < melhor_custo_global)
                {
                    melhor_custo_global = melhor_custo_local;
                    melhor_caminho_global = melhor_caminho_local;
                    melhorou = true;
                }
            }
        }

        if(melhorou)
        {
            custo_atual = melhor_custo_global;
            caminho_atual = melhor_caminho_global;
        }
    } while(melhorou);
}

int Grafo::calcularCustoSemPenalidade(const vector<int>& caminho) 
{
    int custo = 0;
    for (int i = 0; i < caminho.size() - 1; i++) 
    {
        int u = caminho[i], v = caminho[i+1];
        for(auto& aresta : grafo[u]) 
        {
            if(aresta.destino == v) 
            {
                custo += aresta.custo;
                break;
            }
        }
    }
    return custo;
}

void Grafo::BuscaLocalOpenMP_GLS(int &custo_atual, vector<int> &caminho_atual) 
{
    vector<int> graus_originais(tamanho);
    for(int i = 0; i < tamanho; i++) 
    {
        graus_originais[i] = grau(i);
    }

    bool melhorou;
    int melhor_custo_global = custo_atual + lambda * calcularSomaPenalidades(caminho_atual);
    vector<int> melhor_caminho_global = caminho_atual;

    do 
    {
        melhorou = false;

        #pragma omp parallel
        {
            int melhor_custo_local = melhor_custo_global;
            vector<int> melhor_caminho_local = melhor_caminho_global;

            #pragma omp for schedule(dynamic)
            for(int v = 0; v < tamanho; v++) 
            {
                Grafo grafo_local = *this;
                grafo_local.reset_arestas();

                for(int i = 0; i < (int)grafo_local.grafo[v].size() - 1; i++) 
                {
                    Grafo grafo_copia = grafo_local;
                    swap(grafo_copia.grafo[v][i], grafo_copia.grafo[v][i + 1]);

                    vector<int> novo_caminho = {ORIGEM};
                    int novo_custo = 0;
                    vector<int> new_graus = graus_originais;

                    grafo_copia.carteiro_chines(ORIGEM, novo_custo, novo_caminho, new_graus);

                    // Custo com penalidade incluída
                    int custo_com_penalidade = novo_custo + lambda * calcularSomaPenalidades(novo_caminho);

                    if(custo_com_penalidade < melhor_custo_local) 
                    {
                        melhor_custo_local = custo_com_penalidade;
                        melhor_caminho_local = novo_caminho;
                    }
                }
            }

            #pragma omp critical
            {
                if(melhor_custo_local < melhor_custo_global) 
                {
                    melhor_custo_global = melhor_custo_local;
                    melhor_caminho_global = melhor_caminho_local;
                    melhorou = true;
                }
            }
        }

        if(melhorou) 
        {
            caminho_atual = melhor_caminho_global;
            custo_atual = calcularCustoSemPenalidade(melhor_caminho_global); // salvar só o custo puro
        }

    } while(melhorou);
}


void Grafo::caminho_percorrido(vector<int> caminho)
{
    cout << "Caminho percorrido: ";
    for(int i = 0; i < caminho.size(); i++)
    {
        cout << caminho[i] << (i == caminho.size() - 1 ? "\n" : " -> ");
    }
}

void Grafo::caminho_percorrido_2(vector<int> caminho)
{
    cout << "Caminho percorrido: ";
    cout << "{";
    for(int i = 0; i < caminho.size(); i++)
    {
        cout << caminho[i] << (i == caminho.size() - 1 ? "}\n" : ", ");
    }
}

void Grafo::GuidedLocalSearch(int &custo_atual, vector<int> &caminho_atual, int tempo_maximo_minuto) 
{
    auto inicio = high_resolution_clock::now();
    vector<int> melhor_caminho = caminho_atual;
    int melhor_custo = custo_atual;

    while(true)
    {
        auto agora = high_resolution_clock::now();
        auto duracao = duration_cast<seconds>(agora - inicio).count();
        if (duracao >= tempo_maximo_minuto) break;
        
        // Calcular custo aumentado h(s)
        int custo_aumentado = custo_atual + lambda * calcularSomaPenalidades(caminho_atual);

        // Busca Local usando custo aumentado
        int novo_custo = custo_atual;
        vector<int> novo_caminho = caminho_atual;
        BuscaLocalOpenMP_GLS(novo_custo, novo_caminho);

        // Atualizar penalidades se não houve melhoria real
        if(novo_custo >= custo_atual) 
        {
            atualizarPenalidades(novo_caminho);
        } 
        else 
        {
            custo_atual = novo_custo;
            caminho_atual = novo_caminho;
            resetarPenalidades();
        }
    }
}

int Grafo::calcularSomaPenalidades(const vector<int>& caminho) 
{
    int soma = 0;
    for(int i = 0; i < caminho.size() - 1; i++) 
    {
        int u = caminho[i], v = caminho[i + 1];
        soma += penalidades[u][v];
    }
    return soma;
}

void Grafo::atualizarPenalidades(const vector<int>& caminho) 
{
    vector<pair<int, int>> arestas_utilizadas;
    for(int i = 0; i < caminho.size() - 1; i++) 
    {
        arestas_utilizadas.emplace_back(caminho[i], caminho[i + 1]);
    }

    // Calcular utilidades
    double max_util = -INFINITY;
    vector<pair<int, int>> arestas_max_util;
    for(const auto& edge : arestas_utilizadas)
    {
        int u = edge.first, v = edge.second;
        int custo_aresta = 0;
        for(const auto& aresta : grafo[u]) 
        {
            if(aresta.destino == v) 
            {
                custo_aresta = aresta.custo;
                break;
            }
        }

        double util = (custo_aresta) / (1.0 + penalidades[u][v]);
        if(util > max_util) 
        {
            max_util = util;
            arestas_max_util.clear();
            arestas_max_util.push_back(edge);
        } 
        else if(util == max_util) 
        {
            arestas_max_util.push_back(edge);
        }
    }

    // Incrementar penalidades
    for(const auto& edge : arestas_max_util) 
    {
        penalidades[edge.first][edge.second]++;
    }
}

void Grafo::resetarPenalidades() 
{
    for(auto &linha : penalidades) fill(linha.begin(), linha.end(), 0);
}

void Grafo::ACO(int &custo, vector<int> &caminho, int origem, int max_iteracoes, int num_formigas) 
{
    vector<int> melhor_caminho;
    int melhor_custo = INF;

    // Loop principal de iterações
    for(int iter = 0; iter < max_iteracoes; iter++) 
    {
        vector<vector<int>> caminhos_formigas(num_formigas);
        vector<int> custos_formigas(num_formigas, 0);

        // Paralelização do processo das formigas
        #pragma omp parallel for
        for(int f = 0; f < num_formigas; f++) 
        {
            // Criação de cópia local do grafo para cada formiga
            Grafo grafo_formiga = *this;
            grafo_formiga.reset_arestas();
            
            vector<int> caminho_local;
            vector<int> graus(tamanho);
            for(int i = 0; i < tamanho; i++) 
            {
                graus[i] = grafo_formiga.grau(i);
            }
            
            int custo_local = 0;
            int atual = origem;
            bool completou = false;
            int passos = 0;
            caminho_local.push_back(atual);

            // Construção do caminho pela formiga
            while(true) 
            {
                // Seleção de próxima aresta
                vector<pair<int, double>> probabilidades;
                double soma = 0.0;

                // Calcula probabilidades para cada aresta não marcada
                for(const auto& aresta : grafo_formiga.grafo[atual]) 
                {
                    if(!aresta.marcado) 
                    {
                        double feromonio = feromonios[atual][aresta.destino];
                        double heuristica = 1.0 / aresta.custo;
                        double valor = pow(feromonio, alpha) * pow(heuristica, beta);
                        probabilidades.emplace_back(aresta.destino, valor);
                        soma += valor;
                    }
                }

                // Decisão de movimento
                if(!probabilidades.empty()) 
                {
                    // Seleção por roleta
                    random_device rd;
                    mt19937 gen(rd());
                    uniform_real_distribution<> distrib(0.0, soma);
                    double escolha = distrib(gen);
                    
                    double acumulado = 0.0;
                    int proximo = -1;
                    
                    // Encontra próxima aresta baseado na probabilidade
                    for(const auto& p : probabilidades) 
                    {
                        acumulado += p.second;
                        if(acumulado >= escolha)
                        {
                            proximo = p.first;
                            break;
                        }
                    }

                    // Atualiza estado se encontrou próxima aresta
                    if(proximo != -1) 
                    {
                        int custo_aresta_ida;
                        // Marca aresta de ida
                        for(auto& aresta : grafo_formiga.grafo[atual]) 
                        {
                            if(aresta.destino == proximo && !aresta.marcado) 
                            {
                                aresta.marcado = true;
                                graus[atual]--;
                                custo_local += aresta.custo;
                                custo_aresta_ida = aresta.custo;
                                break;
                            }
                        }
                        
                        // Marca aresta de volta (se não direcionado)
                        for(auto& aresta : grafo_formiga.grafo[proximo]) 
                        {
                            if(aresta.destino == atual && !aresta.marcado && aresta.custo == custo_aresta_ida) 
                            {
                                aresta.marcado = true;
                                graus[proximo]--;
                                break;
                            }
                        }
                        
                        caminho_local.push_back(proximo);
                        atual = proximo;
                    }
                    else 
                    {
                        break; // Caso raro: nenhuma aresta válida
                    }
                }
                else 
                {
                    // Usar caminho mínimo se não houver arestas disponíveis
                    int destino = -1;
                    int min_dist = INF;
                    
                    // Encontra vértice mais próximo com arestas não visitadas
                    for(int v = 0; v < tamanho; v++) 
                    {
                        if(v != atual && graus[v] > 0 && dist[atual][v] < min_dist) 
                        {
                            min_dist = dist[atual][v];
                            destino = v;
                        }
                    }

                    if(destino == -1) 
                    {
                        // Se não encontrar, finaliza
                        completou = true;
                        break;
                    }
                    else 
                    {
                        // Adiciona caminho mínimo ao trajeto
                        auto path = restaurar_caminho(atual, destino);
                        for(size_t i = 1; i < path.size(); i++) 
                        {
                            caminho_local.push_back(path[i]);
                            custo_local += dist[path[i-1]][path[i]];
                        }
                        atual = destino;
                    }
                }

                // Verifica se todas as arestas foram cobertas
                bool todas_cobertas = true;
                for(int u = 0; u < tamanho; u++) 
                {
                    for(const auto& aresta : grafo_formiga.grafo[u]) 
                    {
                        if(!aresta.marcado) 
                        {
                            todas_cobertas = false;
                            break;
                        }
                    }
                    if(!todas_cobertas) break;
                }
                if(todas_cobertas) 
                {
                    // Volta para origem se completou
                    auto path = restaurar_caminho(atual, origem);
                    for(size_t i = 1; i < path.size(); i++) 
                    {
                        caminho_local.push_back(path[i]);
                        custo_local += dist[atual][path[i]];
                    }
                    completou = true;
                    break;
                }
            }

            // Atualiza melhor solução global
            #pragma omp critical
            {
                if(completou && custo_local < melhor_custo) 
                {
                    melhor_custo = custo_local;
                    melhor_caminho = caminho_local;
                }
                caminhos_formigas[f] = caminho_local;
                custos_formigas[f] = custo_local;
            }
        }

        // Atualização global de feromônios
        if(melhor_custo != INF) 
        {
            // Evaporação
            for(auto& linha : feromonios) 
            {
                for(auto& val : linha) 
                {
                    val *= (1.0 - taxa_evaporacao);
                }
            }
            
            // Depósito no melhor caminho
            double deposito = 1.0 / melhor_custo;
            for(size_t i = 0; i < melhor_caminho.size() - 1; i++) 
            {
                int u = melhor_caminho[i];
                int v = melhor_caminho[i+1];
                feromonios[u][v] += deposito;
                if(!grafo[u].empty()) // Para grafos não direcionados
                { 
                    feromonios[v][u] += deposito;
                }
            }
        }
    }

    // Retorna resultados pelos parâmetros
    custo = melhor_custo;
    caminho = melhor_caminho;
}

