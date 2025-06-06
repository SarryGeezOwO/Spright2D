TARGET      := Spright2D
SRC_DIR     := src
OBJ_DIR     := obj
BIN_DIR     := bin
SRC_DIRS 	:= src src/core src/engine src/utils
SRC 		:= $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.cpp))
OBJ         := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))
INCLUDES    := -Iinclude -Iinclude/SDL3 -Iinclude/SDL3_image -Iinclude/Eigen
LIBS        := -Llibs -lSDL3 -lSDL3_image
CXX         := ccache g++
CXXFLAGS    := -mwindows $(INCLUDES)
LDFLAGS     := $(LIBS)
EXE         := $(BIN_DIR)/$(TARGET).exe

all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS)

clean:
	rm -rf $(OBJ_DIR)/*.o $(EXE)

run: all
	cd $(BIN_DIR) && ./$(TARGET).exe
