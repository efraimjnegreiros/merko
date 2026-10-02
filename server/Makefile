CXX := g++
CC := gcc
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude -Ithird_party/sqlite3 -Ithird_party/httplib -Ithird_party/json
CFLAGS := -O2

SRC_DIR := src
BUILD_DIR := build
THIRD_PARTY := third_party/sqlite3

# Todas as fontes .cpp, exceto os dois pontos de entrada (main.cpp / main_api.cpp),
# formam a "biblioteca" comum de dominio + DAO + API routes, compartilhada pelos dois binarios.
ALL_SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')
MAIN_CLI := $(SRC_DIR)/main.cpp
MAIN_API := $(SRC_DIR)/main_api.cpp
COMMON_SOURCES := $(filter-out $(MAIN_CLI) $(MAIN_API),$(ALL_SOURCES))

COMMON_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(COMMON_SOURCES))
MAIN_CLI_OBJECT := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(MAIN_CLI))
MAIN_API_OBJECT := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(MAIN_API))

SQLITE_OBJECT := $(BUILD_DIR)/sqlite3.o

TARGET_CLI := sistema_vendas
TARGET_API := sistema_vendas_api

.PHONY: all clean run run-api

all: $(TARGET_CLI) $(TARGET_API)

# Binario de demonstracao (linha de comando, roda o cenario de exemplo e termina)
$(TARGET_CLI): $(SQLITE_OBJECT) $(COMMON_OBJECTS) $(MAIN_CLI_OBJECT)
	$(CXX) $(COMMON_OBJECTS) $(MAIN_CLI_OBJECT) $(SQLITE_OBJECT) -o $@ -lpthread -ldl

# Binario do servidor HTTP (fica no ar ate Ctrl+C)
$(TARGET_API): $(SQLITE_OBJECT) $(COMMON_OBJECTS) $(MAIN_API_OBJECT)
	$(CXX) $(COMMON_OBJECTS) $(MAIN_API_OBJECT) $(SQLITE_OBJECT) -o $@ -lpthread -ldl

$(SQLITE_OBJECT): $(THIRD_PARTY)/sqlite3.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET_CLI) $(TARGET_API) sistema_vendas.db

run: $(TARGET_CLI)
	./$(TARGET_CLI)

run-api: $(TARGET_API)
	./$(TARGET_API)
