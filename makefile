## configurações do compilador
CC := g++
CFLAGS := -g -Wall -Wextra -Wpedantic -Wformat-security -Wconversion -Werror

##  definição de diretórios
INC_DIR := include
OBJ_DIR := obj
BIN_DIR := ./
SRC_DIR := src

## saída
EXEC_NAME := tp2

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
	./$(BIN_DIR)/$(EXEC_NAME) < tests/inputs/testCase01.txt

# Executa os testes
tests: all
	echo -e "\n\e[34;1mTeste 1\e[0m\n"
	./$(BIN_DIR)/$(EXEC_NAME) < tests/inputs/testCase01.txt
	echo -e "\n\e[34;1mTeste 2\e[0m\n"
	./$(BIN_DIR)/$(EXEC_NAME) < tests/inputs/testCase02.txt
	echo -e "\n\e[34;1mTeste 3\e[0m\n"
	./$(BIN_DIR)/$(EXEC_NAME) < tests/inputs/testCase03.txt
	echo -e "\n\e[34;1mTeste 4\e[0m\n"
	./$(BIN_DIR)/$(EXEC_NAME) < tests/inputs/testCase04.txt
	echo -e "\n\e[34;1mTeste 5\e[0m\n"
	./$(BIN_DIR)/$(EXEC_NAME) < tests/inputs/testCase05.txt
	echo -e "\n\e[34;1mTeste 6\e[0m\n"
	./$(BIN_DIR)/$(EXEC_NAME) < tests/inputs/testCase06.txt
	echo -e "\n\e[34;1mTeste 7\e[0m\n"
	./$(BIN_DIR)/$(EXEC_NAME) < tests/inputs/testCase07.txt
	echo -e "\n\e[34;1mTeste 8\e[0m\n"
	./$(BIN_DIR)/$(EXEC_NAME) < tests/inputs/testCase08.txt

# Checa se o programa possui vazamentos de memória, usando valgrind.
memcheck : all
	valgrind --leak-check=full -s ./$(BIN_DIR)/$(EXEC_NAME) < tests/inputs/testCase05.txt

.PHONY: clean