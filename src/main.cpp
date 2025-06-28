#include "algoritmos.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <unistd.h>
#include <fstream>

//leitura do tempo gasto executando o algoritmo
double diffUserTime(struct rusage *start, struct rusage *end) {
  return (double)(end->ru_utime.tv_sec - start->ru_utime.tv_sec) +
         (double)(end->ru_utime.tv_usec - start->ru_utime.tv_usec);
}

//leitura do tempo gasto no kernel-mode
double diffSystemTime(struct rusage *start, struct rusage *end) {
  return (double)(end->ru_stime.tv_sec - start->ru_stime.tv_sec) +
         (double)(end->ru_stime.tv_usec - start->ru_stime.tv_usec);
}

//calculando o tempo total gasto para rodar o programa
double totalSpentTime(struct rusage *start, struct rusage *end){
    return diffSystemTime(start, end) + diffUserTime(start, end);
}

//lendo o arquivo e salvando em um vetor de item, criado na estrutura de dados Algoritmo
void loadData(std::string &path, std::vector<Item> &list, int &wMax){
    std::ifstream arq(path);
    //verifica se o arquivo foi aberto corretamente
    if (!arq){
        std::cout<<"Error!"<<std::endl;
        exit (1);
    }
    int n;
    //pegando o valor máximo
    if (!(arq >> n >> wMax)) {  //verificando se conseguiu ler a linha corretamente
        std::cerr << "Error reading n and wMax from file" << std::endl;
        exit(1);
    }
    //instanciando as variáveis para leitura dos valores
    double w,v;

    for (int i = 0; i < n; i++) {
        if (!(arq >> v >> w)) {    
            std::cerr << "Error reading item " << i << " from file" << std::endl;
            exit(1);
        }
        //adicionando o valor à lista
        list.push_back({unsigned(i), w, v});
    }
}


int main(int argc, char **argv) {
    if(argc < 2){
        std::cerr << "Usage: " << argv[0] << " <path>" << std::endl;
        return 1;
    }
    //Peso Máximo do problema
    int wMax;
    //caminho do arquivo
    std::string path = argv[1];
    //vetor auxiliar
    std::vector<Item> list;
    //lendo o arquivo e salvando em um vetor
    loadData(path,list,wMax);
    //iniciando o struct do contador.
    struct rusage start, end;
        /*
        Essa instancia é para verificar se os valores estão corretos
        */

    std::cout<<"Número de Total de Itens: " << list.size()<<" Peso máximo: "<< wMax<<std::endl;
    std::cout<< "Branch and Bound: " << AlgoritmosMochila::branchAndBound(list, wMax) << std::endl;
    std::cout << "2-aproximativo: " << AlgoritmosMochila::aproximativo(list, wMax) << std::endl;

    //Mensuração do tempo do Branch and Bound padrão
    getrusage(RUSAGE_SELF, &start);
    AlgoritmosMochila::branchAndBound(list,wMax);
    getrusage(RUSAGE_SELF, &end);

    std::cout<<"Tempo Branch and Bound : "<<totalSpentTime(&start, &end)<<std::endl;

    //mensuração do tempo do aproximativo
    getrusage(RUSAGE_SELF, &start);
    AlgoritmosMochila::aproximativo(list, wMax);
    getrusage(RUSAGE_SELF, &end);

    std::cout<<"Tempo Aproximatibo : "<<totalSpentTime(&start, &end)<<std::endl;

    return 0;
}