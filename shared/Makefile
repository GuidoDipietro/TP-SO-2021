SRC_DIR = ./src
_SRC = $(shell find ./src -type f -iname "*.c" -exec basename {} \;)
SRC = $(patsubst %,$(SRC_DIR)/%,$(SRC))

ODIR = ./obj
_OBJ = $(_SRC:.c=.o)
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

HEADERS = $(shell find ./include/*.h)

O_DIR = ./obj
CC = gcc
CFLAGS = -c -o
LINK = -lcommons

.PHONY: makeFolder

build: makeFolder $(OBJ)

./obj/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) $@ $< $(LINK)

makeFolder:
	@if [ ! -d ./obj ]; then \
		mkdir obj; \
	fi

clean:
	rm -rf obj
