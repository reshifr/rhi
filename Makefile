# Reshifr Hash Index
# Make

CC = gcc
FLAGS =
RHI_FLAGS = -std=c99 -O3 -Wall -Wextra -Wshadow -Wpedantic -Wconversion
RHI_DIR = src
LIBS_DIR = libs

all:

static:

# Static library build.
#
ifneq (,$(filter Windows%,$(OS)))
	$(CC) -c $(RHI_DIR)\rhi.c -o $(LIBS_DIR)\rhi.o $(RHI_FLAGS) -I$(RHI_DIR)
	ar rcs $(LIBS_DIR)\librhi.a $(LIBS_DIR)\rhi.o
	del /F $(LIBS_DIR)\rhi.o 1> nul 2> nul || ver > nul
	$(CC) $(LIBS_DIR)\runlib.c -o $(LIBS_DIR)\runlib.exe \
		-L$(LIBS_DIR) -I$(RHI_DIR) -lrhi
	$(LIBS_DIR)\runlib.exe
	del /F $(LIBS_DIR)\runlib.exe 1> nul 2> nul || ver > nul
else
	@$(CC) -c $(RHI_DIR)/rhi.c -o $(LIBS_DIR)/rhi.o $(RHI_FLAGS) -I$(RHI_DIR)
	@ar rcs $(LIBS_DIR)/librhi.a $(LIBS_DIR)/rhi.o
	@rm -rf $(LIBS_DIR)/rhi.o
	@$(CC) $(LIBS_DIR)/runlib.c -o $(LIBS_DIR)/runlib \
		-L$(LIBS_DIR) -I$(RHI_DIR) -lrhi
	@chmod +x $(LIBS_DIR)/runlib
	@./$(LIBS_DIR)/runlib
	@rm -rf $(LIBS_DIR)/runlib
endif
	
dynamic:

# Dynamic library build.
#
ifneq (,$(filter Windows%,$(OS)))
	@$(CC) -c $(RHI_DIR)\rhi.c -o $(LIBS_DIR)\rhi.o $(RHI_FLAGS) \
		-I$(RHI_DIR) -DRHI_EXPORT
	@$(CC) -shared $(LIBS_DIR)\rhi.o -o $(LIBS_DIR)\librhi.dll
	@del /F $(LIBS_DIR)\rhi.o 1> nul 2> nul || ver > nul
	@$(CC) $(LIBS_DIR)\runlib.c -o $(LIBS_DIR)\runlib.exe \
		-L$(LIBS_DIR) -I$(RHI_DIR) -lrhi -DRHI_IMPORT
	@$(LIBS_DIR)\runlib.exe
	@del /F $(LIBS_DIR)\runlib.exe 1> nul 2> nul || ver > nul
else
	@$(CC) -c $(RHI_DIR)/rhi.c -o $(LIBS_DIR)/rhi.o \
		-fPIC $(RHI_FLAGS) -I$(RHI_DIR)
	@$(CC) -shared $(LIBS_DIR)/rhi.o -o $(LIBS_DIR)/librhi.so
	@rm -rf $(LIBS_DIR)/rhi.o
	@$(CC) $(LIBS_DIR)/runlib.c -o $(LIBS_DIR)/runlib \
		-L$(LIBS_DIR) -Wl,-rpath=$(LIBS_DIR) -I$(RHI_DIR) -lrhi
	@chmod +x $(LIBS_DIR)/runlib
	@./$(LIBS_DIR)/runlib
	@rm -rf $(LIBS_DIR)/runlib
endif

clean:

# Remove all existing libraries.
#
ifneq (,$(filter Windows%,$(OS)))
	@del /F $(LIBS_DIR)\librhi.* 1> nul 2> nul || ver > nul
else
	@rm -rf $(LIBS_DIR)/librhi.* &> /dev/null
endif
