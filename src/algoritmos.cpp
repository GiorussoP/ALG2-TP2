#include "algoritmos.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
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

// Implementação do algoritmo 2-aproximativo guloso
// Retorna um valor, no máximo, duas vezes pior que o resultado ótimo que pode ser obtido com a mochila
double AlgoritmosMochila::aproximativoGuloso(std::vector<Item> items, double W) {

    // Ordena por razão valor/peso decrescente
    std::sort(items.begin(), items.end(),[](const Item &a, const Item &b) {
            return (a.v / a.w) > (b.v / b.w);
        });

    // Solução gulosa
    double total_w = 0.0;
    double total_v = 0.0;
    for (auto item : items) {
        if (total_w + item.w <= W) {
            total_w += item.w;
            total_v += item.v;
        }
    }

    // Melhor item isolado que cabe sozinho
    double melhor_item = 0.0;
    for (auto item : items) {
        if (item.w <= W && item.v > melhor_item) {
            melhor_item = item.v;
        }
    }

    // Garante fator 2-aproximativo
    return std::max(total_v, melhor_item);
}

// Implementação do algoritmo 2-aproximativo utilizando FPTAS
double AlgoritmosMochila::aproximativoFPTAS(std::vector<Item> items, double W){ 
    const double EPS = 0.5; // Garante q o algoritmo seja 2-aproximativo
    int valorTotal = 0; 
    int n = (int) items.size();
    double Vmax = 0;
    for(Item item : items){
        Vmax = std::max(Vmax, item.v);
    }

    const double MI = (Vmax*EPS)/n;

    // Transforma os valores do problema para o 2-aprox
    std::vector<int> valorAprox(n);
    for(int i = 0; i < n; i++){
        valorAprox[i] = (int) std::floor(items[i].v / MI);
        valorTotal += valorAprox[i];
    }

    const double INF = std::numeric_limits<double>::infinity();
    std::vector<std::vector<double>> tabela(n + 1, std::vector<double>(valorTotal + 1, INF));
    std::vector<std::vector<bool>> escolhidos(n + 1, std::vector<bool>(valorTotal + 1, false));

    for(int i = 0; i <= n; i++)
        tabela[i][0] = 0;

    for(int i = 1; i <= n; i++){
        int currV = valorAprox[i-1];
        double currW = items[i-1].w;
        for(int V = 0; V <= valorTotal; V++){
            // Não escolhe o item i
            double semItem = tabela[i-1][V];
            // Escolhe o item i
            double comItem = INF;
            if(V >= currV && tabela[i-1][V-currV] < INF) comItem = currW + tabela[i-1][V-currV];
            double melhorEscolha = std::min(semItem, comItem);
            tabela[i][V] = melhorEscolha;
            escolhidos[i][V] = (comItem < semItem);

        }
    }

    // Pega o melhor valor transformado que respeite o limite da mochila
    int ansTransformado = 0;
    for(int V = valorTotal; V >= 0; V--){
        if(tabela[n][V] <= W){
            ansTransformado = V;
            break;
        }
    }

    // Encontrar o real valor colocado na mochila encontrado pelo aproximativo
    double ans = 0.0;
    int V = ansTransformado;  // Começa do melhor V transformado
    for (int i = n; i > 0; i--){
        if (escolhidos[i][V]){
            ans += items[i-1].v;
            V -= valorAprox[i-1];
        }
    }

    return ans;  // Soma dos valores do itens selecionados pelo aproximativo, garantindo output >= (1-EPS)OPT
}

