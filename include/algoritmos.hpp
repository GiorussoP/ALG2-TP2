
#ifndef ALGORITMOS_H

#include <vector>

// Estrutura que representa um item no problema da mochila binária
struct Item {
    double w;
    double v;
};


// Clase que organiza os algoritmos de mochila binária
class AlgoritmosMochila {
    private:
        // Nó da árvore de busca do branch and bound
        struct Node {
            unsigned int level;     // Nível na árvore de busca
            double total_w;         // Peso total dos itens na mochila
            double total_v;         // Valor total dos itens na mochila
            double ub;           // Limite superior do valor total possível
        };
        // Comparador para os nós do branch and bound, na fila de prioridade
        struct node_cmp {
            bool operator()(const AlgoritmosMochila::Node& a, const AlgoritmosMochila::Node& b) const {
                return a.total_v < b.total_v;
            }
        };
    public:
        static double branchAndBound(std::vector<Item> items, double W);
        static double aproximativoFPTAS(std::vector<Item> items, double W);
        static double aproximativoGuloso(std::vector<Item> items, double W);
};

#endif