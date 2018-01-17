CXX := g++
CXX_FLAGS := -O3 -Wall -fPIC -shared
LD_FLAGS	:= -fPIC -shared
TARGET := libstd.so

SRC_DIR := src
INC_DIR := inc
SRC := $(patsubst ./%,%,$(shell find $(SRC_DIR) -type f -name "*.cpp"))
OBJ_DIR := obj
AR := ar

obj:=$(patsubst %cpp,%o,$(SRC))
DIRS:=$(patsubst ./%,%,$(shell find . -type f -name "*.cpp" | sed 's/\/[^\/]*cpp//'))

$(shell mkdir -p $(patsubst %,$(OBJ_DIR)/%,$(DIRS)))

.PHONY:clean all

all:$(TARGET)

ifneq ($(MAKECMDGOALS),clean)
-include $(SRC:.cpp=.d)
endif

$(obj):%.o:%.cpp
	$(CXX) -c $(CXX_FLAGS) $< -I$(INC_DIR) -o $(OBJ_DIR)/$@ -MMD -MF $(OBJ_DIR)/$*.d -MP

$(TARGET):$(obj)
	$(CXX) $(LD_FLAGS) -o $@ $(patsubst %,$(OBJ_DIR)/%,$^)

clean:
	rm -rf *.o *.d $(OBJ_DIR) $(TARGET)
