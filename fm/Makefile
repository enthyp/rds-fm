TARGET := main
BUILD_DIR := ./build
INCL_DIR := ./include

SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:src/%.cpp=$(BUILD_DIR)/%.o)

CFLAGS := -Wall -g


.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CFLAGS) -lpthread -lrtlsdr $^ -o $@

$(OBJ): CFLAGS += -I$(INCL_DIR)
$(OBJ): CFLAGS += -c
$(OBJ): $(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR):
	@mkdir -p $@

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) $(TARGET)
