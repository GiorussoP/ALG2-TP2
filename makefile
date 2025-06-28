## configurações do compilador
CC := g++
CFLAGS := -g -Wall -Wextra -Wpedantic -Wformat-security -Wconversion -Werror

##  definição de diretórios
INC_DIR := include
OBJ_DIR := obj
BIN_DIR := bin
SRC_DIR := src
TESTES_DIR:=tests

LARGE := $(wildcard $(TESTES_DIR)/large_scale_trated/*)
LOW := $(wildcard $(TESTES_DIR)/low-dimensional/*)

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

run : all
	./$(BIN_DIR)/$(EXEC_NAME)

# Executa os testes
testslow: all
	@rm -f low_results.txt
	@for file in $(LOW); do \
		# modo 1 -------------------------------------------- \
		echo "=== $$file (mode 1) ===" ; \
		./$(BIN_DIR)/$(EXEC_NAME) "$$file" 1 >> low_results.txt ; \
		# modo 2 -------------------------------------------- \
		echo "=== $$file (mode 2) ===" ; \
		./$(BIN_DIR)/$(EXEC_NAME) "$$file" 2 >> low_results.txt ; \
	done


testslarge: all
	@rm -f large_results.txt
	@for file in $(LARGE); do \
		# modo 1 -------------------------------------------- \
		echo "=== $$file (mode 1) ===" ; \
		./$(BIN_DIR)/$(EXEC_NAME)  "$$file" 1 >> large_results.txt; \
		# modo 2 -------------------------------------------- \
		echo "=== $$file (mode 2) ===" ; \
		./$(BIN_DIR)/$(EXEC_NAME)  "$$file" 2 >> large_results.txt; \
	done


# Checa se o programa possui vazamentos de memória, usando valgrind.
memcheck : all
	valgrind --leak-check=full -s ./$(BIN_DIR)/$(EXEC_NAME) < tests/inputs/testCase05.txt

.PHONY: clean