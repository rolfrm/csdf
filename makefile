# Config
CC       = clang
OPT      = -Os -g1
CFLAGS   = -Isrc/ $(OPT) -MMD
LDFLAGS  = -L. -framework Cocoa -framework IOKit
LIBS     = -lm
TARGET   = isosdf
SRC_DIR  = src

# Sources
CFILES   = $(wildcard $(SRC_DIR)/*.c)
MFILES   = $(wildcard $(SRC_DIR)/*.m)
OBJECTS  = $(CFILES:.c=.o) $(MFILES:.m=.o)
DEPS     = $(OBJECTS:.o=.d)

# Default target
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) $(LIBS) -o $@

# Compilation rules
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) $< -o $@

$(SRC_DIR)/%.o: $(SRC_DIR)/%.m
	$(CC) -c $(CFLAGS) $< -o $@

# Release build
release: OPT = -O4 -g0
release: all

# Utilities
clean:
	rm -f $(TARGET) $(OBJECTS) $(DEPS)

test: $(TARGET)
	./$(TARGET) test

# Auto include dependencies
-include $(DEPS)

.PHONY: all release clean test
