# front end do merko em c++
# compila tudo q ta em src/ e gera o executavel merko_front

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2 -Iinclude -Ithird_party/httplib -Ithird_party/json

SRC_DIR   := src
BUILD_DIR := build

FONTES  := $(shell find $(SRC_DIR) -name '*.cpp')
OBJETOS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(FONTES))

TARGET := merko_front

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJETOS)
	$(CXX) $(OBJETOS) -o $@ -lpthread

# se mexer em algum .hpp recompila tudo pra nao dar problema
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(wildcard include/*.hpp)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# roda o front na 3000 usando a api na 8080
run: $(TARGET)
	./$(TARGET) 3000 localhost 8080
