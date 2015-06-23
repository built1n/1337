PROGRAM_NAME = 1337

CC = cc
INSTALL = install

SRC := $(wildcard src/*.c) $(wildcard lib1337/src/*.c)
OBJ := $(SRC:.c=.o)

GIT_VERSION := $(shell git describe --abbrev=8 --always --dirty)

PREFIX = /usr
BINDIR = $(PREFIX)/bin

INCLUDES = -I lib1337/include -I src
LIBS = -l1337 -lm -lz -lSDL2 -lSDL2_ttf

HEADERS := $(wildcard src/*.h)

LIBSRC := $(wildcard lib1337/src/*.c)
LIBOBJ := $(LIBSRC:.c=.o)
LIBINC := $(wildcard lib1337/include/*.h) $(wildcard lib1337/src/*.h)

CFLAGS = -Og -g -Wall -Wextra -std=gnu99 -DVERSION_INFO=\"$(GIT_VERSION)\" $(INCLUDES) -fPIC

$(PROGRAM_NAME): $(OBJ) Makefile $(HEADERS) $(LIBINC) lib1337/src/lib1337.so
	@echo "LD $@"
	@$(CC) $(OBJ) -o $@ $(CFLAGS) $(LIBS)

lib1337/src/lib1337.so: $(LIBOBJ) $(LIBINC)
	@echo "LD $@"
	@$(CC) $(CFLAGS) -o $@ $(LIBOBJ) $(LIBS) -shared

%.o: %.c Makefile $(HEADERS)
	@echo "CC $<"
	@$(CC) $(CFLAGS) -c $< -o $@

install: all
	@echo "INSTALL $(PROGRAM_NAME)"
	@install $(PROGRAM_NAME) $(BINDIR)

clean:
	@echo "Cleaning build directory..."
	@rm -f $(OBJ) $(LIBOBJ) $(PROGRAM_NAME)

all: $(PROGRAM_NAME)
