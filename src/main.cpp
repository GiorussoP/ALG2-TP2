#include "algoritmos.hpp"
#include <iostream>
int main() {
    std::vector<Item> itens;

    /*
        TESTE SIMPLES DO BRANCH AND BOUND - problema low-dimensional-f7
    */
    itens.push_back({0,31,70});
    itens.push_back({1,10,20});
    itens.push_back({2,20,39});
    itens.push_back({3,19,37});
    itens.push_back({4,4,7});
    itens.push_back({5,3,5});
    itens.push_back({6,6,10});
    std::cout<< "Branch and Bound: " << AlgoritmosMochila::branchAndBound(itens, 50) << std::endl;
    std::cout << "2-aproximativo: " << AlgoritmosMochila::aproximativo(itens, 50) << std::endl;


    
    // Ler entrada, salvar em vetor de itens:
    /* 
    qtde itens, capacidade mochila,
    para cada item:
    peso, valor
    */

    // Começar a medir tempo
    //AlgoritmosMochila::branchAndBound();
    // Parar a medição de tempo

    // Começar a medir tempo
    //AlgoritmosMochila::aproximativo();
    // Parar a medição de tempo

    return 0;
}