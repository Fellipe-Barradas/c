CC = gcc
CFLAGS = -Iinclude -Wall -Werror
OBJDIR = obj
BINDIR = bin
SRCDIR = src
INCLUDEDIR = include

SRC = $(SRCDIR)/main.c $(SRCDIR)/error.c
OBJ = $(OBJDIR)/main.o $(OBJDIR)/error.o
TARGET = $(BINDIR)/server

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^

$(OBJDIR)/main.o: $(SRCDIR)/main.c $(INCLUDEDIR)/error.h
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/error.o: $(SRCDIR)/error.c $(INCLUDEDIR)/error.h
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJDIR) $(BINDIR)

.PHONY: all clean
