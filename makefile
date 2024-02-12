
OUTPUT := Stratego

SRC_DIR := src
BLD_DIR := build

CXX_SRCS := $(shell find $(SRC_DIR) -iname "*.cpp")
CXX_OBJS := $(CXX_SRCS:$(SRC_DIR)/%.cpp=$(BLD_DIR)/%.cpp.o)
INC_DIR  := $(shell find $(SRC_DIR) -type d)
DEPS     := $(CXX_OBJS.o=.d)

INC_FLAGS := $(addprefix -I,$(INC_DIR))
LD_FLAGS := -lncurses
CXX_FLAGS := -O2 -g -MD -MMD -Wall -Werror -Wextra $(INC_FLAGS) $(LDFLAGS) --std=c++20

.PHONY: all test clean

all: $(BLD_DIR)/$(OUTPUT)

clean:
	-rm -r $(BLD_DIR)

test: $(BLD_DIR)/$(OUTPUT)
	$<

$(BLD_DIR)/$(OUTPUT): $(CXX_OBJS)
	g++ $(CXX_FLAGS) -o $@ $^ $(LD_FLAGS)

$(BLD_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	g++ $(CXX_FLAGS) -c $< -o $@ 

-include $(DEPS)
