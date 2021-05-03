SRC_DIR = ./src
_SRC = $(shell find ./src -type f -iname "*.c" -exec basename {} \;)
SRC = $(patsubst %,$(SRC_DIR)/%,$(SRC))

ODIR = ./obj
_OBJ = $(_SRC:.c=.o)
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

HEADERS = $(shell find ./include/*.h)

SHARED = $(shell find ../shared/obj/*.o)

O_DIR = ./obj
CC = gcc
CFLAGS = -c -o
LINK = -lcommons -lreadline -lpthread -lnivel-gui -lncurses

.PHONY: makeFolder

build: makeShared makeFolder $(OBJ)
	$(CC) main.c $(OBJ) $(SHARED) $(LINK)

./obj/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) $@ $< $(LINK)

makeShared:
	make -C ../shared

makeFolder:
	@if [ ! -d ./obj ]; then \
		mkdir obj; \
	fi

clean:
	rm -rf obj
	rm -rf *.out
	make clean -C ../shared
