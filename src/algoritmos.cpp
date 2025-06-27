#include "algoritmos.hpp"
#include <algorithm>
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