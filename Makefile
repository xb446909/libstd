CXX := g++

ifeq ($(shell uname), Linux)
LD_FLAGS	:= -fPIC -shared -g
TARGET := libstd.so
endif

ifeq ($(shell uname), Darwin)
LD_FLAGS	:= -fPIC -dynamiclib
TARGET := libstd.dylib
endif

CXX_FLAGS := -O3 -Wall $(LD_FLAGS) -std=gnu++11 -D__OPENCV_BUILD

SRC_DIR := src
INC_FLAGS := -Iinc -Iwin -Isrc/opencv -Isrc/zlib
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
	$(CXX) -c $(CXX_FLAGS) $< $(INC_FLAGS) -o $(OBJ_DIR)/$@ -MMD -MF $(OBJ_DIR)/$*.d -MP

$(TARGET):$(obj)
	$(CXX) $(LD_FLAGS) -o $@ $(patsubst %,$(OBJ_DIR)/%,$^)

clean:
	rm -rf *.o *.d $(OBJ_DIR) $(TARGET)

