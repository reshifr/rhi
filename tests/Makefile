CC ?= cc
FLAGS += \
	-O2 \
	-std=c99 \
	-Wall \
	-Wconversion \
	-Wextra \
	-Wshadow

RHI_FLAGS += \
	-Wpedantic

ifneq (,$(filter Windows%,$(OS)))
	RHI_DIR := ..\rhi
else
	RHI_DIR := ../rhi
endif

TEST_DIR := .
INCLUDES := -I $(RHI_DIR)
LIBS +=

ifneq (,$(filter Windows%,$(OS)))
	RHI_HEADERS := $(shell dir /A-D /B /S "$(RHI_DIR)\*.h")
	TEST_HEADERS := $(shell dir /A-D /B /S "$(TEST_DIR)\*.h")
	RHI_OBJS := $(patsubst %.c,%.o,$(shell dir /A-D /B /S "$(RHI_DIR)\*.c"))
	TEST_OBJS := $(patsubst %.c,%.o,$(shell dir /A-D /B /S "$(TEST_DIR)\*.c"))
	TARGET := main.exe
else
	LIBS += -lm
	RHI_HEADERS := $(shell find $(RHI_DIR) -type f -name "*.h")
	TEST_HEADERS := $(shell find $(TEST_DIR) -type f -name "*.h")
	RHI_OBJS := $(patsubst %.c,%.o,$(shell find $(RHI_DIR) -type f -name "*.c"))
	TEST_OBJS := $(patsubst %.c,%.o,$(shell find $(TEST_DIR) -type f -name "*.c"))
	TARGET := main
endif

HEADERS := $(RHI_HEADERS) $(TEST_HEADERS)
OBJS := $(RHI_OBJS) $(TEST_OBJS)

.PHONY: all clean
.NOTPARALLEL: $(TARGET)

all: $(RHI_DIR)/rhi.o $(OBJS) $(TARGET)

clean:
ifneq (,$(filter Windows%,$(OS)))
	@del $(OBJS) $(TARGET)
else
	@rm -f $(OBJS) $(TARGET)
endif

$(RHI_DIR)/rhi.o: $(RHI_DIR)/rhi.c $(RHI_HEADERS)
	@$(CC) -c $< -o $@ $(FLAGS) $(RHI_FLAGS) $(INCLUDES)

%.o: %.c $(HEADERS)
	@$(CC) -c $< -o $@ $(FLAGS) $(INCLUDES)

$(TARGET): $(OBJS)
	@$(CC) $(OBJS) -o $(TARGET) $(LIBS)
