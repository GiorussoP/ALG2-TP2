## configurações do compilador
CC := g++
CFLAGS := -g -Wall -Wextra -Wpedantic -Wformat-security -Wconversion -Werror

##  definição de diretórios
INC_DIR := include
OBJ_DIR := obj
BIN_DIR := bin
SRC_DIR := src

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
tests: all
	echo -e "\n\e[34;1mTeste 1\e[0m\n"
	./$(BIN_DIR)/$(EXEC_NAME) < tests/inputs/testCase01.txt
	


# Checa se o programa possui vazamentos de memória, usando valgrind.
memcheck : all
	valgrind --leak-check=full -s ./$(BIN_DIR)/$(EXEC_NAME) < tests/inputs/testCase05.txt

.PHONY: clean