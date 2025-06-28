#include "algoritmos.hpp"
#include <algorithm>
#include <cmath>
#include<limits>
#include <utility>
#include <queue>
#include <iostream>


// Implementação do algoritmo Branch and Bound
// Retorna o valor máximo que pode ser obtido com a mochila
double AlgoritmosMochila::branchAndBound(std::vector<Item> items, double W){
    std::vector<std::pair<unsigned int, double>> v_w;
    
    // Calculando o valor por peso
    for (unsigned int i = 0; i < items.size(); ++i)
        v_w.push_back({i,items[i].v / items[i].w});

    // Ordenando por valor/peso em ordem decrescente
    sort(v_w.begin(), v_w.end(), [](const auto& a, const auto& b) {
        return a.second > b.second; 
    });
    
    // Algoritmo branch and bound com best first search e fila de priridade
    std::priority_queue<Node,std::vector<Node>,node_cmp> pq;
    pq.push({0,0.0, 0.0,v_w[0].second * W});

    double best_sol = 0;
    while(!pq.empty()){
        Node curr_node = pq.top(); pq.pop();    // Obtendo o melhor para best-first-search

        // Descartando nós que não podem melhorar a solução
        if (curr_node.ub <= best_sol)
            continue;
        
        // Atualizando se for uma solução melhor
        if(curr_node.level == items.size()) {
            if (curr_node.total_v > best_sol)
                best_sol = curr_node.total_v;
            continue;
        }

        // Obtendo o índice do próximo item
        unsigned int next_id = v_w[curr_node.level].first;

        // Filho 1: Incluindo o item atual
        double total_w = curr_node.total_w + items[next_id].w;
        if (total_w <= W) {
            double total_v = curr_node.total_v + items[next_id].v;
            double bound = total_v + v_w[curr_node.level + 1].second * (W - total_w);
            pq.push({curr_node.level + 1, total_w, total_v, bound});
        }

        // Filho 2: Não incluindo o item atual
        pq.push({curr_node.level + 1, curr_node.total_w, curr_node.total_v, curr_node.total_v + v_w[curr_node.level + 1].second * (W - curr_node.total_w)});
    }
    return best_sol;
}

// Implementação do algoritmo 2-aproximativo FPTAS
// Retorna um valor, no máximo, duas vezes pior que o resultado ótimo que pode ser obtido com a mochila
double AlgoritmosMochila::aproximativo(std::vector<Item> items, double W){
    const double EPS = 0.5;       // Garante q o algoritmo seja 2-aproximativo
    int valorTotal = 0;
    int n = (int) items.size();

    double Vmax = 0;
    for(Item item : items){
        Vmax = std::max(Vmax, item.v);
    }

    const double MI = (Vmax*EPS)/n;

    // Transforma os valores do problema para o 2-aprox
    std::vector<int> valueAprox(n);
    for(int i = 0; i < n; i++){
        valueAprox[i] = (int) std::floor(items[i].v / MI);
        valorTotal += valueAprox[i];
    }

    const double INF = std::numeric_limits<double>::infinity();
    std::vector<std::vector<double>> tabela(n + 1, std::vector<double>(valorTotal + 1, INF));

    for(int i = 0; i <= n; i++)
        tabela[i][0] = 0;

    for(int i = 1; i <= n; i++){
        int currV = valueAprox[i-1];
        double currW = items[i-1].w;
        for(int V = 0; V <= valorTotal; V++){
            // Não escolhe o item i
            double melhorEscolha = tabela[i-1][V];
            // Escolhe o item i
            if(V >= currV && tabela[i-1][V-currV] < INF) melhorEscolha = std::min(melhorEscolha, currW + tabela[i-1][V-currV]);
            tabela[i][V] = melhorEscolha;
        }
    }

    // Pega o melhor valor transformado que respeite o limite da mochila
    int ans = 0;
    for(int V = valorTotal; V >= 0; V--){
        if(tabela[n][V] <= W){
            ans = V;
            break;
        }
    }

    // Converte o valor para a instância do problema original, garantindo: output >= (1-EPS)OPT
    return ans * MI;
}