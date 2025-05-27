#include <SFML/Graphics.hpp>
#include <vector>
#include <fstream>
#include <cmath>
#include <iostream>
#include <set>              
#include <random>           
#include <utility>      
#include <chrono>   

#include "PCC.h"

#define W 1280
#define H 720

using namespace std;
using namespace std::chrono;

struct Aresta_Draw 
{
    int origem;
    int destino;
    int peso;
    bool orientada;
};

struct Vertice_Draw
{
    sf::CircleShape shape;
    sf::Text label;
};

std::vector<Vertice_Draw> vertices;

std::vector<Vertice_Draw> gerar_vertices_em_circulo(int n, const sf::Font& fonte) 
{
    std::vector<Vertice_Draw> vertices;
    float raio = 250.f;
    sf::Vector2f centro(W / 2.f, H / 2.f);

    for(int i = 0; i < n; ++i) 
    {
        float ang = i * 2 * M_PI / n;
        float x = centro.x + raio * std::cos(ang);
        float y = centro.y + raio * std::sin(ang);

        Vertice_Draw v;
        v.shape = sf::CircleShape(8.f);
        v.shape.setFillColor(sf::Color::Cyan);
        v.shape.setOrigin(8.f, 8.f);
        v.shape.setPosition(x, y);

        v.label.setFont(fonte);
        v.label.setString(std::to_string(i));
        v.label.setCharacterSize(12);
        v.label.setFillColor(sf::Color::Black);

        sf::FloatRect bounds = v.label.getLocalBounds();
        v.label.setOrigin(bounds.width / 2, bounds.height / 2 + bounds.top);
        v.label.setPosition(x, y);

        vertices.push_back(v);
    }

    return vertices;
}

std::vector<Vertice_Draw> gerar_vertices_aleatorios(int n, const sf::Font& fonte) 
{
    std::vector<Vertice_Draw> vertices;
    std::set<std::pair<int, int>> usados;
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist_x(100, W - 100);
    std::uniform_int_distribution<int> dist_y(100, H - 100);

    for(int i = 0; i < n; ++i) 
    {
        Vertice_Draw v;

        int x, y;
        do 
        {
            x = dist_x(rng);
            y = dist_y(rng);
        } while(usados.count({x / 10, y / 10})); // evita pontos muito próximos
        usados.insert({x / 10, y / 10});

        v.shape = sf::CircleShape(8.f);
        v.shape.setFillColor(sf::Color::Cyan);
        v.shape.setOrigin(8.f, 8.f);
        v.shape.setPosition(x, y);

        v.label.setFont(fonte);
        v.label.setString(std::to_string(i));
        v.label.setCharacterSize(12);
        v.label.setFillColor(sf::Color::White);

        sf::FloatRect bounds = v.label.getLocalBounds();
        v.label.setOrigin(bounds.width / 2, bounds.height / 2 + bounds.top);
        v.label.setPosition(x, y);

        vertices.push_back(v);
    }

    return vertices;
}


void desenhar_peso(sf::RenderWindow& window, const sf::Vector2f& a, const sf::Vector2f& b, int peso, const sf::Font& fonte) 
{
    // Ponto médio
    sf::Vector2f meio = (a + b) / 2.f;

    // Vetor direção da linha
    sf::Vector2f direcao = b - a;

    // Vetor perpendicular (sentido 90 graus)
    sf::Vector2f perp(-direcao.y, direcao.x);

    // Normaliza o vetor perpendicular
    float comprimento = std::sqrt(perp.x * perp.x + perp.y * perp.y);
    if(comprimento != 0)
        perp /= comprimento;

    // Deslocamento perpendicular (pode ajustar o 10.f)
    sf::Vector2f deslocado = meio + perp * 10.f;

    // Texto
    sf::Text texto;
    texto.setFont(fonte);
    texto.setString(std::to_string(peso));
    texto.setCharacterSize(12);
    texto.setFillColor(sf::Color::White);

    // Centraliza o texto
    sf::FloatRect bounds = texto.getLocalBounds();
    texto.setOrigin(bounds.width / 2, bounds.height / 2 + bounds.top);
    texto.setPosition(deslocado);

    window.draw(texto);
}



void ler_grafo(const std::string& nome_arquivo, std::vector<Aresta_Draw>& arestas, int& num_vertices) 
{
    std::ifstream arq(nome_arquivo);
    if(!arq) 
    {
        std::cerr << "Erro ao abrir o arquivo\n";
        exit(1);
    }

    int num_arestas;
    arq >> num_vertices >> num_arestas;

    for(int i = 0; i < num_arestas; ++i) 
    {
        int a, b, peso;
        char tipo;
        arq >> a >> tipo >> b >> peso;
        arestas.push_back({a, b, peso, tipo == '>'});
    }
}


void aresta_nao_orientada(sf::RenderWindow& window, const sf::CircleShape& v1, const sf::CircleShape& v2, sf::Color cor = sf::Color(200, 200, 200)) 
{
    sf::Vertex linha[] = 
    {
        sf::Vertex(v1.getPosition(), cor),
        sf::Vertex(v2.getPosition(), cor)
    };
    window.draw(linha, 2, sf::Lines);
}

void aresta_orientada(sf::RenderWindow& window, const sf::Vector2f& origem, const sf::Vector2f& destino, float grossura = 2.f, sf::Color cor = sf::Color::White, bool deslocar = false) 
{
    sf::Vector2f direcao = destino - origem;
    float comprimento = std::sqrt(direcao.x * direcao.x + direcao.y * direcao.y);
    sf::Vector2f unit = direcao / comprimento;
    sf::Vector2f destino_recuado = destino - unit * 8.f;

    sf::Vector2f offset(0.f, 0.f);
    if(deslocar) 
    {
        sf::Vector2f perp(-unit.y, unit.x);  // vetor perpendicular
        offset = perp * 15.f; // deslocamento
    }

    sf::Vector2f origem_deslocada = origem + offset;
    sf::Vector2f destino_deslocada = destino_recuado + offset;

    float comprimento_linha = std::hypot(destino_deslocada.x - origem_deslocada.x, destino_deslocada.y - origem_deslocada.y);
    float angulo = std::atan2(destino_deslocada.y - origem_deslocada.y, destino_deslocada.x - origem_deslocada.x) * 180 / M_PI;

    sf::RectangleShape linha(sf::Vector2f(comprimento_linha, grossura));
    linha.setPosition(origem_deslocada);
    linha.setRotation(angulo);
    linha.setFillColor(cor);
    window.draw(linha);

    sf::ConvexShape seta;
    seta.setPointCount(3);
    seta.setPoint(0, sf::Vector2f(0, 0));
    seta.setPoint(1, sf::Vector2f(-10, 5));
    seta.setPoint(2, sf::Vector2f(-10, -5));
    seta.setFillColor(cor);
    seta.setPosition(destino_deslocada);
    seta.setRotation(angulo);
    window.draw(seta);
}


std::map<std::pair<int, int>, int> contar_uso_arestas(const std::vector<int>& caminho, const std::vector<Aresta_Draw>& arestas) 
{
    std::map<std::pair<int, int>, int> uso;

    for(size_t i = 0; i + 1 < caminho.size(); ++i) 
    {
        int u = caminho[i];
        int v = caminho[i + 1];
        bool encontrada = false;

        for(const auto& a : arestas) 
        {
            if(a.origem == u && a.destino == v && a.orientada) {
                uso[{u, v}]++;
                encontrada = true;
                break;
            } 
            else if(!a.orientada) 
            {
                if((a.origem == u && a.destino == v) || (a.origem == v && a.destino == u)) 
                {
                    uso[{std::min(u, v), std::max(u, v)}]++;
                    encontrada = true;
                    break;
                }
            }
        }

        if(!encontrada) 
        {
            std::cerr << "⚠️ Caminho inválido: " << u << " → " << v << " não encontrado no grafo.\n";
        }
    }

    return uso;
}

std::vector<sf::Vector2f> ler_posicoes(const std::string& nome_arquivo) 
{
    std::ifstream arq(nome_arquivo);
    std::vector<sf::Vector2f> posicoes;

    float margem = 80.f;  // Margem maior
    float x, y;
    while(arq >> x >> y) {
        // Escala ajustada para ocupar 90% da janela
        float px = (x + 1.5f) / 3.f * (W - 2 * margem) + margem;
        float py = (y + 1.5f) / 3.f * (H - 2 * margem) + margem;
        posicoes.emplace_back(px, py);
    }

    return posicoes;
}

std::vector<Vertice_Draw> gerar_vertices_com_posicoes(const std::vector<sf::Vector2f>& posicoes, const sf::Font& fonte) 
{
    std::vector<Vertice_Draw> vertices;

    for(size_t i = 0; i < posicoes.size(); ++i) 
    {
        Vertice_Draw v;
        // Em gerar_vertices_com_posicoes():
        v.shape = sf::CircleShape(15.f); // Aumente o raio para 12
        v.shape.setOrigin(15.f, 15.f);   // Atualize a origem
        v.shape.setFillColor(sf::Color::Cyan);
        v.shape.setPosition(posicoes[i]);

        v.label.setFont(fonte);
        v.label.setString(std::to_string(i));
        v.label.setCharacterSize(12);
        v.label.setFillColor(sf::Color::Black);

        sf::FloatRect bounds = v.label.getLocalBounds();
        v.label.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
        v.label.setPosition(posicoes[i]);

        vertices.push_back(v);
    }

    return vertices;
}

void aplicar_solucao_carteiro_chines(std::vector<Vertice_Draw>& vertices, const std::vector<int>& caminho) 
{
    std::map<int, int> visitas;
    std::vector<sf::Color> cores_visita = 
    {
        sf::Color::Red,
        sf::Color::Green,
        sf::Color::Blue,
        sf::Color::Yellow
    };

    for (int v : caminho) 
    {
        int count = visitas[v]++;
        int cor_index = std::min(count, (int)cores_visita.size() - 1);
        vertices[v].shape.setFillColor(cores_visita[cor_index]);
    }
}

std::vector<sf::Color> cores_uso = 
{
    sf::Color::Red,
    sf::Color::Green,
    sf::Color::Blue,
    sf::Color::Yellow
};

int main() 
{
    std::vector<Aresta_Draw> arestas;
    int num_vertices;
    
    Grafo grafo("grafo2.txt");
    ler_grafo("grafo2.txt", arestas, num_vertices);

    int tamanho = grafo.get_tamanho();

    if(tamanho <= ORIGEM)
    {
        cout << "Erro! O vértice de origem não representa nenhum vértice, mude para uma origem pelo menos menor que: " << tamanho << endl;
        return -1;
    }

    int custo_total = 0;
    vector <int> caminho, graus(tamanho);

    for(int i = 0; i < tamanho ; i++)
    {
        graus[i] = grafo.grau(i);
    }

    caminho.push_back(ORIGEM);

    // Carteiro Chinês com algoritmo guloso de menor aresta
    auto inicio_guloso = high_resolution_clock::now();
    grafo.carteiro_chines(ORIGEM, custo_total, caminho, graus);
    auto fim_guloso = high_resolution_clock::now();
    auto duracao_guloso = duration_cast<milliseconds>(fim_guloso - inicio_guloso);

    cout << "Custo total: " << custo_total << endl;
    cout << "Tempo do algoritmo guloso: " << duracao_guloso.count() << " ms" << endl;

    // grafo.caminho_percorrido_2(caminho);
    // grafo.imprimir_grafo_com_bool();
    cout << endl;

    // Busca Local
    auto inicio_omp = high_resolution_clock::now();
    grafo.BuscaLocalOpenMP(custo_total, caminho);
    auto fim_omp = high_resolution_clock::now();
    auto duracao_omp = duration_cast<milliseconds>(fim_omp - inicio_omp);

    cout << "Custo total: " << custo_total << endl;
    cout << "Tempo da busca local: " << duracao_omp.count() << " ms" << endl;
    // grafo.caminho_percorrido_2(caminho);
    cout << endl;

    // Executar ACO
    int intera = 50; 
    int formigas = 20;
    grafo.reset_arestas();
    auto inicio_aco = high_resolution_clock::now();
    grafo.ACO(custo_total, caminho, ORIGEM, intera, formigas);
    auto fim_aco = high_resolution_clock::now();
    auto duracao_aco = duration_cast<milliseconds>(fim_aco - inicio_aco);


    cout << "Custo total: " << custo_total << endl;
    cout << "Tempo do ACO: " << duracao_aco.count() << " ms" << endl;
    cout << "Número de Interações: " << intera << endl;
    cout << "Número de formigas: " << formigas << endl;
    // grafo.caminho_percorrido_2(caminho);
    // grafo.imprimir_grafo_com_bool();
    cout << endl;

    // Executar GLS
    auto inicio_gls = high_resolution_clock::now();
    grafo.GuidedLocalSearch(custo_total, caminho);
    auto fim_gls = high_resolution_clock::now();
    auto duracao_gls = duration_cast<milliseconds>(fim_gls - inicio_gls);
    
    cout << "Tempo GLS: " << duracao_gls.count() << " ms" << endl;
    cout << "Custo total: " << custo_total << endl;

    // Caminho Final
    cout << endl;
    grafo.caminho_percorrido_2(caminho);

    sf::RenderWindow window(sf::VideoMode(W, H), "Grafo");

    sf::Font fonte;

    std::vector<sf::Vector2f> posicoes = ler_posicoes("posicoes.txt");
    std::vector<Vertice_Draw> vertices = gerar_vertices_com_posicoes(posicoes, fonte);

    if(!fonte.loadFromFile("arial.ttf"))
    {
        std::cerr << "Erro ao carregar fonte.\n";
        return 1;
    }

    auto uso_arestas = contar_uso_arestas(caminho, arestas);
    sf::Text texto_custo;
    texto_custo.setFont(fonte);
    texto_custo.setString("Custo: " + std::to_string(custo_total));
    texto_custo.setCharacterSize(16);
    texto_custo.setFillColor(sf::Color::White);
    texto_custo.setPosition(W - 150, 10); // canto superior direito (ajuste se quiser)

    while(window.isOpen()) 
    {
        sf::Event event;
        while(window.pollEvent(event)) 
        {
            if(event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color(51, 45, 86));

        for(const auto& a : arestas) 
        {
            int u = a.origem;
            int v = a.destino;
            auto key = a.orientada ? std::make_pair(u, v) : std::make_pair(std::min(u, v), std::max(u, v));

            // Verifica se existe a aresta reversa (v → u)
            bool deslocar = false;
            if(a.orientada) 
            {
                for(const auto& a2 : arestas) 
                {
                    if(a2.origem == v && a2.destino == u && a2.orientada) 
                    {
                        deslocar = true;
                        break;
                    }
                }
            }

            
            int usos = uso_arestas[key];
            sf::Color cor = (usos > 0 && usos <= cores_uso.size()) ? cores_uso[usos - 1] : sf::Color::White;

            if(a.orientada)
                aresta_orientada(window, vertices[u].shape.getPosition(), vertices[v].shape.getPosition(), 1.f, cor, deslocar);
            else
                aresta_nao_orientada(window, vertices[u].shape, vertices[v].shape, cor);

            desenhar_peso(window, vertices[u].shape.getPosition(), vertices[v].shape.getPosition(), a.peso, fonte);
        }
        
        for(const auto& v : vertices) 
        {
            window.draw(v.shape);
            window.draw(v.label);
        }

        window.draw(texto_custo);
        window.display();
    }

    return 0;
}