CXX = g++
CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -lpthread

BUILD_DIR = ./build

TARGET = ContarPalabras
TEST_TARGET = UnitTests

OBJECTS = HashMapConcurrente.o CargarArchivos.o

all: build $(BUILD_DIR)/$(TARGET)

test: build $(BUILD_DIR)/$(TEST_TARGET)
	$(BUILD_DIR)/$(TEST_TARGET)

$(BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDFLAGS)

$(BUILD_DIR)/%: src/%.cpp $(OBJECTS:%=$(BUILD_DIR)/%)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

.PHONY: all test build clean test_debug debug benchmark

build:
	@mkdir -p $(BUILD_DIR)

test_debug: CXXFLAGS += -DDEBUG -g
test_debug: $(BUILD_DIR)/$(TEST_TARGET)

debug: CXXFLAGS += -DDEBUG -g
debug: all

benchmark: CXXFLAGS += -lrt -D__BENCHMARK__
benchmark: all

clean:
	-@rm -rvf $(BUILD_DIR)/*
