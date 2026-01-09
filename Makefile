# Compiler
CC := cc
TARGET := te

# Find all .c files recursively
SRCS := $(shell find src -name '*.c')

# Default flags (debug)
CFLAGS := $(shell pkg-config --cflags lua raylib) -g -Wall -Wextra
LIBS := $(shell pkg-config --libs lua raylib) -lm

# Default target (debug)
all: $(TARGET)

# Release build (optimized)
build: CFLAGS += -O2
build: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)

