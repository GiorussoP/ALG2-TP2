## configurações do compilador
CC := g++
CFLAGS := -g -Wall -Wextra -Wpedantic -Wformat-security -Wconversion -Werror

##  definição de diretórios
INC_DIR := include
OBJ_DIR := obj
BIN_DIR := bin
SRC_DIR := src
TESTES_DIR := tests
RESULTS_DIR := results

LARGE := $(wildcard $(TESTES_DIR)/tratado_large_scale/*)
LOW := $(wildcard $(TESTES_DIR)/tratado_low_dimensional/*)

## saída
EXEC_NAME := tp2.exe

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')

OBJS := $(SRCS:%= $(OBJ_DIR)/%.o)

all: $(BIN_DIR)/$(EXEC_NAME)

$(BIN_DIR)/$(EXEC_NAME) : $(OBJS)
	mkdir -p $(dir $@)
	$(CC) $^ -o $@
$(OBJ_DIR)/$(SRC_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I./$(INC_DIR)/ -c $^ -o $@

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BIN_DIR)

cleanresults:
	rm -rf $(RESULTS_DIR)

# Executa os testes
$(RESULTS_DIR)/low_results.txt:
	mkdir -p $(RESULTS_DIR)
	@for file in $(LOW); do \
		# modo 1 -------------------------------------------- \
		echo "\033[33m=== $$file (mode 1) ===\033[0m" ; \
		./$(BIN_DIR)/$(EXEC_NAME) "$$file" 1 >> $(RESULTS_DIR)/low_results.txt ; \
		# modo 2 -------------------------------------------- \
		echo "\033[33m=== $$file (mode 2) ===\033[0m" ; \
		./$(BIN_DIR)/$(EXEC_NAME) "$$file" 2 >> $(RESULTS_DIR)/low_results.txt ; \
		# modo 3 -------------------------------------------- \
		echo "\033[33m=== $$file (mode 3) ===\033[0m" ; \
		./$(BIN_DIR)/$(EXEC_NAME) "$$file" 3 >> $(RESULTS_DIR)/low_results.txt ; \
	done

$(RESULTS_DIR)/large_results.txt:
	mkdir -p $(RESULTS_DIR)
	@for file in $(LARGE); do \
		echo "\033[33m=== $$file (mode 3) ===\033[0m" ; \
		timeout 30m ./$(BIN_DIR)/$(EXEC_NAME)  "$$file" 3 >> $(RESULTS_DIR)/large_results.txt || echo "skipped $$file, timeout"; \
	done


testslow: all
	mkdir -p $(RESULTS_DIR)
	@echo "Iniciando testes com arquivos de baixa dimensão..."
	@$(MAKE) --always-make $(RESULTS_DIR)/low_results.txt
	@echo "Testes com arquivos de baixa dimensão concluídos."

testslarge: all
	mkdir -p $(RESULTS_DIR)
	@echo "Iniciando testes com arquivos de alta dimensão..."
	@echo "Aguarde, isso pode levar algum tempo...\033"
	@$(MAKE) --always-make $(RESULTS_DIR)/large_results.txt
	@echo "Testes com arquivos de alta dimensão concluídos."

tests : testslow testslarge
	@echo "Testes concluídos."

results: all $(RESULTS_DIR)/low_results.txt $(RESULTS_DIR)/large_results.txt
	mkdir -p $(RESULTS_DIR)/images
	@echo "Transformando resultados..."
	python3 ./src/transform_results.py
	@echo "Resultados transformados e salvos em $(RESULTS_DIR)."

.PHONY: clean tests testslow testslarge results cleanresults
