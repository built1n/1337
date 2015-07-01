PROGRAM_NAME = 1337

CC = cc
INSTALL = install

SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)

VERSION = v0.0.2

GIT_VERSION := $(shell git describe --abbrev=8 --always --dirty)

PREFIX = /usr
BINDIR = $(PREFIX)/bin
LIBDIR = $(PREFIX)/lib

INCLUDES = -I lib1337/include -I src
LIBS = -lm -lz -lSDL2 -lSDL2_ttf -lcurses

HEADERS := $(wildcard src/*.h)

CFLAGS = -Og -g -Wall -Wextra -std=gnu99 -DVERSION_INFO=\"$(VERSION)\ \($(GIT_VERSION)\)\" -DPROGRAM_NAME=\"$(PROGRAM_NAME)\" $(INCLUDES) -fPIC

LIBSRC := $(wildcard lib1337/src/*.c)
LIBOBJ := $(LIBSRC:.c=.o)
LIBINC := $(wildcard lib1337/include/*.h) $(wildcard lib1337/src/*.h)

all: $(PROGRAM_NAME) $(PROGRAM_NAME)_static

$(PROGRAM_NAME): Makefile $(OBJ) $(HEADERS) lib1337/src/lib1337.so
	@echo "LD $@"
	@$(CC) $(OBJ) -o $@ $(CFLAGS) $(LIBS) -L lib1337/src -l1337
	@echo "WARNING: you need to run \`make install\` for the program to work properly"

$(PROGRAM_NAME)_static: Makefile $(OBJ) $(HEADERS) $(LIBOBJ)
	@echo "LD $@"
	@$(CC) $(OBJ) -o $@ $(CFLAGS) $(LIBS) $(LIBOBJ)

lib1337/src/lib1337.so: Makefile $(LIBOBJ) $(LIBINC)
	@echo "LD $@"
	@$(CC) $(CFLAGS) $(LIBOBJ) -o $@ $(LIBS) -shared

%.o: %.c Makefile $(HEADERS)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

install: all
	@echo "INSTALL $(PROGRAM_NAME)"
	@install $(PROGRAM_NAME) $(BINDIR)
	@echo "INSTALL lib1337/src/lib1337.so"
	@install lib1337/src/lib1337.so $(LIBDIR)

clean:
	@echo "Cleaning build directory..."
	@rm -f $(OBJ) $(LIBOBJ) $(PROGRAM_NAME) $(PROGRAM_NAME)_static lib1337/src/lib1337.so
