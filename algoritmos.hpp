
#ifndef ALGORITMOS_H

#include <vector>

struct Item {
    unsigned int id;
    double w;
    double v;
};


class AlgoritmosMochila {

public:
    static int branchAndBound(std::vector<Item> items, double W) {
        // Implementação do algoritmo Branch and Bound
        // Retorna o valor máximo que pode ser obtido com a mochila
    }
    static int aproximativo(std::vector<Item> items, double W){
        
    }
};

#endif