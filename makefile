# Compiler
CPP = g++

# Compiler Option
CXXFLAGS = -std=c++17 -Wall -g

# Source File Directory
SRC_DIR = ./src

# Object File Directory
OBJ_DIR = ./obj

# Test File Directory
TEST_DIR = ./tests

SRCS = $(notdir $(wildcard $(SRC_DIR)/*.cpp))
OBJS = $(SRCS:.cpp=.o)
OBJECTS = $(addprefix $(OBJ_DIR)/, $(OBJS))
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)
TESTS = $(basename $(notdir $(TEST_SRCS)))

TARGET = main
TEST_TARGET = $(addprefix $(TEST_DIR)/, $(TESTS))

all: $(TARGET) $(TEST_TARGET)
bin : $(TARGET)
test : $(TEST_TARGET)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

$(TEST_TARGET): $(TEST_DIR)/%_test: $(OBJECTS) $(TEST_DIR)/%_test.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

.PHONY: all clean echo
clean:
	rm -f $(OBJECTS) $(TARGET) $(TEST_TARGET)
echo:
	@echo TARGET:			$(TARGET)
	@echo TEST_TARGET:		$(TEST_TARGET)
	@echo SRCS:				$(SRCS)
	@echo OBJS:				$(OBJS)
	@echo OBJECTS:			$(OBJECTS)
	@echo TESTS:			$(TESTS)