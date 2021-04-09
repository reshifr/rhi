# Reshifr Hash Index
# Make

CC := cc

FLAGS += -std=c99 -O2 -Wall -Wconversion -Wextra -Wshadow
RHI_FLAGS += -Wpedantic -DRHI_PRIME_METHODX
TEST_FLAGS +=

RHI_DIR := ../src
TEST_DIR := .
BUILD_DIR := build

INCLUDES += -I$(RHI_DIR)
LIBS +=
TARGET := main

# Add the executable suffix for Windows
#
ifneq (,$(filter Windows%,$(OS)))
	TARGET := $(TARGET).exe
endif

RHI_HEADERS = \
	$(RHI_DIR)/rhi.h

TEST_HEADERS = \
	$(TEST_DIR)/tests.h \
	$(TEST_DIR)/utils.h

HEADERS := $(RHI_HEADERS) $(TEST_HEADERS)

OBJS = \
	$(BUILD_DIR)/rhi.o \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/utils.o \
	$(BUILD_DIR)/setinit.o \
	$(BUILD_DIR)/setreserve.o \
	$(BUILD_DIR)/setinsert.o

.PHONY: all clean
.NOTPARALLEL: $(TARGET)

all: $(OBJS) $(TARGET)

clean:
# Change `/` to `\` for Windows
#
ifneq (,$(filter Windows%,$(OS)))
	del $(subst /,\,$(OBJS) $(TARGET))
else
	rm -f $(OBJS) $(TARGET)
endif

# Compile the source on the src directory
#
$(BUILD_DIR)/rhi.o: $(RHI_DIR)/rhi.c $(RHI_HEADERS)
	$(CC) -c $< -o $@ $(FLAGS) $(RHI_FLAGS) $(INCLUDES)

# Compile the source on the tests directory
#
$(BUILD_DIR)/main.o: $(TEST_DIR)/main.c $(HEADERS)
	$(CC) -c $< -o $@ $(FLAGS) $(TEST_FLAGS) $(INCLUDES)

$(BUILD_DIR)/utils.o: $(TEST_DIR)/utils.c $(HEADERS)
	$(CC) -c $< -o $@ $(FLAGS) $(TEST_FLAGS) $(INCLUDES)

# Compile the source on the tests/set directory
#
$(BUILD_DIR)/setinit.o: $(TEST_DIR)/set/init.c $(HEADERS)
	$(CC) -c $< -o $@ $(FLAGS) $(TEST_FLAGS) $(INCLUDES)

$(BUILD_DIR)/setreserve.o: $(TEST_DIR)/set/reserve.c $(HEADERS)
	$(CC) -c $< -o $@ $(FLAGS) $(TEST_FLAGS) $(INCLUDES)

$(BUILD_DIR)/setinsert.o: $(TEST_DIR)/set/insert.c $(HEADERS)
	$(CC) -c $< -o $@ $(FLAGS) $(TEST_FLAGS) $(INCLUDES)

# Create executable
#
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)
