CXX := g++
CXX_FLAGS := -O3 -Wall
LD_FLAGS	:= rcs
TARGET := libstd.a

SRC_DIR := src
INC_DIR := inc
SRC := $(patsubst ./%,%,$(shell find . -type f -name "*.cpp"))
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
	$(AR) $(LD_FLAGS) $@ $(patsubst %,$(OBJ_DIR)/%,$^)

clean:
	rm -rf *.o *.d $(OBJ_DIR) $(TARGET)
