#include <bits/stdc++.h>

using namespace std;

const int INF = INT_MAX;
const int ORIGEM = 0;

class Grafo
{
    private:
        // Grafo como lista de adjacências sendo um pair de inteiro com pair de inteiro e booleano 
        // para adicionar o custo junto e se a aresta já foi marcada
        vector<vector<pair<int, pair<int, bool>>>> grafo;
        int tamanho;
    public:
        Grafo(string nomeArquivo)
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
        }

        void adicionar_aresta_direcionadas(int origem, int destino, int custo)
        {
            grafo[origem].push_back({destino, {custo, false}});
        }

        void adicionar_arestas_bidirecionadas(int origem, int destino, int custo)
        {
            grafo[origem].push_back({destino, {custo, false}});
            grafo[destino].push_back({origem, {custo, false}});
        }

        void imprimir_grafo()
        {
            cout << "A impressão do grafo segue o padrão:" << endl;
            cout << "origem: (destino, custo) -> ..." << endl;
            for(int i = 0; i < grafo.size(); i++)
            {
                cout << i << ": ";
                for(int j = 0; j < grafo[i].size(); j++)
                {
                    cout << "(" << grafo[i][j].first << ", " << grafo[i][j].second.first << ")" << (j == grafo[i].size() - 1 ? "" : " -> "); 
                }
                cout << endl;
            }
        }  
        
        void imprimir_grafo_com_bool()
        {
            cout << "A impressão do grafo segue o padrão:" << endl;
            cout << "origem: (destino, custo, foi marcado) -> ..." << endl;
            for(int i = 0; i < grafo.size(); i++)
            {
                cout << i << ": ";
                for(int j = 0; j < grafo[i].size(); j++)
                {
                    cout << "(" << grafo[i][j].first << ", " << grafo[i][j].second.first << ", " << (grafo[i][j].second.second ? "True" : "False") << ")" << (j == grafo[i].size() - 1 ? "" : " -> "); 
                }
                cout << endl;
            }
        }     
        
        void dijkstra(int vertice_inicial, vector <int> & distancias, vector <int> & predecessores)
        {
            distancias.assign(tamanho, INF);
            predecessores.assign(tamanho, -1);
            vector <bool> vertices_marcados(tamanho, false);

            distancias[vertice_inicial] = 0;

            for(int i = 0; i < tamanho; i++)
            {
                int vertice_selecionado = -1;
                for(int j = 0; j < tamanho; j++) // j = vertice_candidato
                {
                    if(!vertices_marcados[j] && (vertice_selecionado == -1 || distancias[j] < distancias[vertice_selecionado]))
                    {
                        vertice_selecionado = j;
                    }
                }

                if(distancias[vertice_selecionado] == INF) break;
                
                vertices_marcados[vertice_selecionado] = true;
                for(auto aresta : grafo[vertice_selecionado])
                {
                    int destino = aresta.first;
                    int custo = aresta.second.first;

                    if(distancias[vertice_selecionado] + custo < distancias[destino])
                    {
                        distancias[destino] = distancias[vertice_selecionado] + custo;
                        predecessores[destino] = vertice_selecionado;
                    }
                }
            }
        }
        
        int get_tamanho()
        {
            return tamanho;
        }

        vector <int> restaurar_caminho(int origem, int destino, vector <int>  const& predecessores) 
        {
            vector <int> caminho;
            
            for(int vertice = destino; vertice != origem; vertice = predecessores[vertice]) 
            {
                caminho.push_back(vertice);
            }

            caminho.push_back(origem);
        
            reverse(caminho.begin(), caminho.end());
            return caminho;
        }

        int grau(int vertice)
        { 
            return grafo[vertice].size(); 
        }

        void carteiro_chines(int vertice_atual, int &custo, vector <int>& caminho,vector <int> & graus)
        {
            // todas as arestas marcadas?
            vector <int>  distancias, predecessores; 
            bool todas_marcadas = true;
            for(int vertice = 0 ; vertice < tamanho && todas_marcadas; vertice++)
            {
                for(auto &aresta : grafo[vertice]) 
                { 
                    if(!aresta.second.second) 
                    {
                        todas_marcadas = false; 
                        break;
                    } 
                }
            }
    
            if(todas_marcadas) // Calculo final para voltar para a origem utilizando do Dijkstra para achar o menor caminho
            { 
                if(vertice_atual != ORIGEM)
                { 
                    dijkstra(vertice_atual, distancias, predecessores);
                    auto path = restaurar_caminho(vertice_atual, ORIGEM, predecessores);
                    if(path.size() > 1)
                    { 
                        caminho.insert(caminho.end(), path.begin() + 1, path.end()); 
                        custo += distancias[ORIGEM]; 
                    }
                } 
                return; 
            }

            // greedy aresta mais barata não marcada
            int melhor_aresta = INF, proximo_vertice = -1;
            for(auto &aresta : grafo[vertice_atual])
            {
                if(!aresta.second.second && aresta.second.first < melhor_aresta)
                { 
                    melhor_aresta = aresta.second.first; 
                    proximo_vertice = aresta.first; 
                }
            } 
            if(proximo_vertice != -1) // Entra aqui se achou alguma aresta que ainda não foi percorrida
            {
                // marca ambos sentidos
                for(auto &aresta : grafo[vertice_atual])
                { 
                    if(aresta.first == proximo_vertice && !aresta.second.second)
                    {
                        aresta.second.second = true;
                        graus[vertice_atual]--; 
                        break; 
                    }
                }
    
                for(auto &aresta : grafo[proximo_vertice]) 
                {
                    if(aresta.first == vertice_atual && !aresta.second.second)
                    { 
                        aresta.second.second=true;
                        graus[proximo_vertice]--;
                        break; 
                    }
                }
    
                // decrementa grau em ambos vértices
                caminho.push_back(proximo_vertice);
                custo += melhor_aresta;
                carteiro_chines(proximo_vertice, custo, caminho, graus);
            } 
            else // Aqui se o vértice atual não tem arestas novas para percorrer mais, então ele calcula Dijkstra para o próximo vértice com arestar não marcadas
            {
                // Dijkstra até vértice com graus > 0 e != vertice_atual
                dijkstra(vertice_atual, distancias, predecessores);
                int distancia_minima = INF, vertice_escolhido = -1;

                for(int vertice = 0; vertice < tamanho; vertice++) 
                {
                    if(vertice != vertice_atual && graus[vertice] > 0 && distancias[vertice] < distancia_minima)
                    { 
                        distancia_minima = distancias[vertice]; 
                        vertice_escolhido = vertice; 
                    }
                }
                if(vertice_escolhido < 0) return;
    
                auto path = restaurar_caminho(vertice_atual, vertice_escolhido, predecessores);
                if(path.size() > 1)
                { 
                    caminho.insert(caminho.end(), path.begin() + 1, path.end()); 
                    custo += distancia_minima; 
                }
                carteiro_chines(vertice_escolhido, custo, caminho, graus);
            }
        }
};

int main()
{
    Grafo grafo("grafo.txt");
    grafo.imprimir_grafo();

    int tamanho = grafo.get_tamanho();
    int custo_total = 0;
    vector <int> caminho, graus(tamanho);

    for(int i = 0; i < tamanho ; i++)
    {
        graus[i] = grafo.grau(i);
    }

    caminho.push_back(ORIGEM);

    grafo.carteiro_chines(ORIGEM, custo_total, caminho, graus);
    
    cout << "Custo total: " << custo_total << endl;
    
    cout << "Caminho percorrido: ";
    for(int i = 0; i < caminho.size(); i++)
    {
        cout << caminho[i] << (i == caminho.size() - 1 ? "\n" : " -> ");
    }

    // grafo.imprimir_grafo_com_bool();

    return 0;
}