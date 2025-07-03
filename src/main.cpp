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

//lendo o arquivo e salvando em um vetor de item, criado na estrutura de dados
void loadData(std::string &path, std::vector<Item> &list, int &w_cap,double &sol){
    std::ifstream arq(path);
    //verifica se o arquivo foi aberto corretamente
    if (!arq){
        std::cout<<"Erro!"<<std::endl;
        exit (1);
    }
    int n;
    // lendo quantidade de itens e capacidade
    if (!(arq >> sol >> n >> w_cap)) {  //verificando se conseguiu ler corretamente
        std::cerr << "Erro lendo sol, n e w!" << std::endl;
        exit(1);
    }
    //instanciando as variáveis para leitura dos valores
    double w,v;

    for (int i = 0; i < n; i++) {
        if (!(arq >> v >> w)) {    
            std::cerr << "Erro lendo item " << i << "!" << std::endl;
            exit(1);
        }
        //adicionando o valor à lista
        list.push_back({w, v});
    }
}

static void printMemMetrics(const char* tag) {
    struct rusage ru{};
    getrusage(RUSAGE_SELF, &ru);

    std::cout << "=== Métricas de Memória [" << tag << "] ===\n"
              << "* Uso aproximado (rusage): "     << ru.ru_ixrss   << " KB\n"
              << "* Dados não compartilhados: "    << ru.ru_idrss   << " KB\n"
              << "* Stack não compartilhado: "     << ru.ru_isrss   << " KB\n"
              << "* Memória residente real (RSS): " << ru.ru_maxrss << " KB\n"
              << "* Pico de memória: "             << ru.ru_maxrss << " KB\n";
}


void printItemInfo(std::vector<Item>& items) {
    double  w_mean = 0,
            w_max = 0,
            v_mean = 0,
            v_max = 0;

    for (auto& item : items) {
        w_mean += item.w;
        v_mean += item.v;
        w_max = std::max(w_max, item.w);
        v_max = std::max(v_max, item.v);
    }
    w_mean /= (double)items.size();
    v_mean /= (double)items.size();

    std::cout << "* Número de itens: " << items.size() << "\n"
              << "* Peso médio: " << w_mean << "\n"
              << "* Peso máximo: " << w_max << "\n"
              << "* Valor médio: " << v_mean << "\n"
              << "* Valor máximo: " << v_max << "\n";
}

int main(int argc, char **argv) {
    if(argc < 3){
        std::cerr << "Uso: " << argv[0] << " <caminho> <modo>\n"
                  << "       <modo> = 1 (branch‑and‑bound) | 2 (approximativo FPTAS) | 3 (aproximativo guloso)\n";
        return 1;
    }
    // Solução ótima do problema
    double sol;
    // Peso Máximo do problema
    int w_cap;
    // Caminho do arquivo
    std::string path = argv[1];
    // Modo de execução do algoritmo 1 - Branch and Bound, 2 - Aproximativo FPTAS, 3 - Aproximativo Guloso
    int modo = std::stoi(argv[2]);
    // Vetor auxiliar
    std::vector<Item> list;
    // Lendo o arquivo e salvando em um vetor
    loadData(path,list,w_cap,sol);
    // Iniciando o struct do contador.
    struct rusage start, end;

    if(modo == 1){
        // Medindo tempo do Branch and Bound padrão
        getrusage(RUSAGE_SELF, &start);
        double resultado = AlgoritmosMochila::branchAndBound(list, w_cap);
        getrusage(RUSAGE_SELF, &end);
        printItemInfo(list);
        std::cout << "* Solução ótima: " << sol << "\n";
        std::cout <<"* Capacidade da Mochila: " << w_cap << "\n";
        std::cout<< "Branch and Bound: " << resultado << std::endl;
        printMemMetrics("BB");
        std::cout<<"Tempo Branch and Bound : "<< totalSpentTime(&start, &end)<<std::endl;
    }else if(modo == 2){

        // Medindo tempo do aproximativo
        getrusage(RUSAGE_SELF, &start);
        double resultado = AlgoritmosMochila::aproximativoFPTAS(list, w_cap);
        getrusage(RUSAGE_SELF, &end);
        printItemInfo(list);
        std::cout << "* Solução ótima: " << sol << "\n";
        std::cout <<"* Capacidade da Mochila: " << w_cap << "\n";
        std::cout << "2-aproximativo-fptas: " << resultado << std::endl;
        printMemMetrics("fptas");
        std::cout<<"Tempo Aproximativo FPTAS: "<< totalSpentTime(&start, &end)<<std::endl;
    }
    else if(modo == 3){
        // Medindo tempo do aproximativo
        getrusage(RUSAGE_SELF, &start);
        double resultado = AlgoritmosMochila::aproximativoGuloso(list, w_cap);
        getrusage(RUSAGE_SELF, &end);
        printItemInfo(list);
        std::cout << "* Solução ótima: " << sol << "\n";
        std::cout <<"* Capacidade da Mochila: " << w_cap << "\n";
        std::cout << "2-aproximativo-guloso: " << resultado << std::endl;
        printMemMetrics("guloso");
        std::cout<<"Tempo Aproximativo Guloso: "<< totalSpentTime(&start, &end)<<std::endl;
    }
    std::cout << std::endl;
    

    return 0;
}