PROGRAM_NAME = 1337

CC = cc
INSTALL = install

SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)

GIT_VERSION := $(shell git describe --abbrev=8 --always --dirty)

CFLAGS = -Isrc/ -O2 -g -Wall -Wextra -std=gnu99 -DVERSION_INFO=\"$(GIT_VERSION)\"

PREFIX = /usr
BINDIR = $(PREFIX)/bin

LIBS = -lm -lSDL2 -lz -lSDL2_ttf

HEADERS := $(wildcard src/*.h)


$(PROGRAM_NAME): $(OBJ) Makefile $(HEADERS)
	@echo "LD $@"
	@$(CC) $(OBJ) -o $@ $(CFLAGS) $(LIBS)

%.o: %.c Makefile $(HEADERS)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

install: all
	@echo "INSTALL $(PROGRAM_NAME)"
	@install $(PROGRAM_NAME) $(BINDIR)

clean:
	@echo "Cleaning build directory..."
	@rm -f $(OBJ) $(PROGRAM_NAME)

all: $(PROGRAM_NAME)
