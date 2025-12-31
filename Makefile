CC := cc
TARGET := te
SRCS := $(wildcard src/*.c)
CFLAGS := $(shell pkg-config --cflags lua raylib)
LIBS := $(shell pkg-config --libs lua raylib) -lm

$(TARGET): $(SRCS)
	cc -o $(TARGET) $(SRCS) $(CFLAGS) $(LIBS)
