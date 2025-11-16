CC = clang++
STD = -Wall -Wextra -std=c++17 -lncurses
TARGET = game

OBJ_DIR = ./obj/
SRC_DIR = ./src/

SRC = $(wildcard $(SRC_DIR)*.cpp)
OBJ = $(SRC:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)

$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(STD)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $< -o $@ $(STD)

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(TARGET)

.PHONY: clean
