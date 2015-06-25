PROGRAM_NAME = 1337

CC = cc
INSTALL = install

SRC := $(wildcard src/*.c) $(wildcard lib1337/src/*.c)
OBJ := $(SRC:.c=.o)

GIT_VERSION := $(shell git describe --abbrev=8 --always --dirty)

PREFIX = /usr
BINDIR = $(PREFIX)/bin
LIBDIR = $(PREFIX)/lib

INCLUDES = -I lib1337/include -I src
LIBS = -lm -lz -lSDL2 -lSDL2_ttf -lcurses

HEADERS := $(wildcard src/*.h)

CFLAGS = -Og -g -Wall -Wextra -std=gnu99 -DVERSION_INFO=\"$(GIT_VERSION)\" $(INCLUDES) -fPIC

$(PROGRAM_NAME)_static: Makefile $(OBJ) $(HEADERS) $(LIBINC) $(LIBOBJ)
	@echo "LD $@"
	@$(CC) $(OBJ) -o $@ $(CFLAGS) $(LIBS) $(LIBOBJ)

lib1337/src/lib1337.so: $(LIBOBJ) $(LIBINC)
	@$(MAKE) -C lib1337

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
	@rm -f $(OBJ) $(LIBOBJ) $(PROGRAM_NAME)_static lib1337/src/lib1337.so

all: $(PROGRAM_NAME)_static
