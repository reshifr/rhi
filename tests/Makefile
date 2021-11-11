# Reshifr Hash Index
# Make

CC = gcc
CXX = g++

FLAGS =
RHI_FLAGS += -std=c99 -O3 -Wall -Wextra -Wshadow -Wpedantic -Wconversion
TEST_FLAGS += -std=c++17 -O3

RHI_DIR := ../src
TEST_DIR := .
BUILD_DIR := build

INCLUDES += -I$(RHI_DIR)
LIBS +=
TARGET := main

# If the compiler used is GCC.
#
ifneq (,$(and $(filter $(CC),gcc),$(filter $(CXX),g++)))
	TEST_FLAGS += -fopenmp
	LIBS += -lgomp
endif

# Add the executable suffix for Windows.
#
ifneq (,$(filter Windows%,$(OS)))
	TARGET := $(TARGET).exe
endif

RHI_HEADERS = \
	$(RHI_DIR)/rhi.h

TEST_HEADERS = \
	$(TEST_DIR)/utils.hpp

HEADERS := $(RHI_HEADERS) $(TEST_HEADERS)

OBJS = \
	$(BUILD_DIR)/rhi.o \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/set/init.o \
	$(BUILD_DIR)/set/reserve.o \
	$(BUILD_DIR)/set/insert.o \
	$(BUILD_DIR)/set/search.o

.PHONY: all check clean

all: check $(OBJS) $(TARGET)

# Run target.
#
ifneq (,$(filter Windows%,$(OS)))
	@$(TARGET)
else
	@chmod +x $(TARGET)
	@./$(TARGET)
endif

check:

# If the build directory doesn't exist, create it.
#
ifneq (,$(filter Windows%,$(OS)))
	@mkdir $(BUILD_DIR)\set $(BUILD_DIR)\map 2> nul || ver > nul
else
	@mkdir -p $(BUILD_DIR)/set $(BUILD_DIR)/map &> /dev/null
endif

clean:

# Remove the build directory and target.
#
ifneq (,$(filter Windows%,$(OS)))
	@del /f /s /q $(TARGET) 1> nul 2> nul || ver > nul
	@rmdir /s /q $(BUILD_DIR) 1> nul 2> nul || ver > nul
else
	@rm -rf $(BUILD_DIR) $(TARGET) &> /dev/null
endif

# Compile the source on src.
#
$(BUILD_DIR)/rhi.o: $(RHI_DIR)/rhi.c $(RHI_HEADERS)
	$(CC) -c $< -o $@ $(FLAGS) $(RHI_FLAGS) $(INCLUDES)

# Compile the source on tests.
#
$(BUILD_DIR)/main.o: $(TEST_DIR)/main.cpp $(HEADERS)
	$(CXX) -c $< -o $@ $(TEST_FLAGS) $(INCLUDES)

# Compile the source on tests/set.
#
$(BUILD_DIR)/set/init.o: $(TEST_DIR)/set/init.cpp $(HEADERS)
	$(CXX) -c $< -o $@ $(TEST_FLAGS) $(INCLUDES)

$(BUILD_DIR)/set/reserve.o: $(TEST_DIR)/set/reserve.cpp $(HEADERS)
	$(CXX) -c $< -o $@ $(TEST_FLAGS) $(INCLUDES)

$(BUILD_DIR)/set/insert.o: $(TEST_DIR)/set/insert.cpp $(HEADERS)
	$(CXX) -c $< -o $@ $(TEST_FLAGS) $(INCLUDES)

$(BUILD_DIR)/set/search.o: $(TEST_DIR)/set/search.cpp $(HEADERS)
	$(CXX) -c $< -o $@ $(TEST_FLAGS) $(INCLUDES)

# Create executable.
#
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)
