# ------------------------------
# Neocities CLI Makefile (Portable)
# ------------------------------

# Default compiler
CC ?= gcc

# Compiler flags
CFLAGS ?= -Wall -O2 -Iinclude

# Libraries
LIBS ?= -lcurl -ljansson

# Source and target
SRC = \
    src/main.c \
    src/info.c    

BIN = neocities

# Detect Windows (MSYS2 / MinGW)
ifeq ($(OS),Windows_NT)
    RM = del /Q
    BIN_EXT = .exe
else
    RM = rm -f
    BIN_EXT =
endif

# Build executable
all: $(BIN)$(BIN_EXT)

$(BIN)$(BIN_EXT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Clean build files
clean:
	$(RM) $(BIN)$(BIN_EXT)
