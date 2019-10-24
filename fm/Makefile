TARGET := main
SRC_DIR := ./src
BUILD_DIR := ./build
INCL_DIR := ./include

SRC := $(wildcard $(SRC_DIR)/**/*.cpp) $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(SRC:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJ:.o=.d)

CFLAGS := -Wall -g


.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CFLAGS) -lpthread -lrtlsdr $^ -o $@

-include $(DEPS)

$(BUILD_DIR)/%.d: CFLAGS += -I$(INCL_DIR)
$(BUILD_DIR)/%.d: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	@set -e; rm -f $@; \
	$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$;

$(OBJ): CFLAGS += -I$(INCL_DIR)
$(OBJ): CFLAGS += -c
$(OBJ): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR):
	@mkdir -p $@

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) $(TARGET)
