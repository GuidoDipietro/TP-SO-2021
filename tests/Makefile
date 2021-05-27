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

CFLAGS = -c

LINK = -lcommons -lreadline -lpthread -lnivel-gui -lncurses -lcunit

OUT_NAME = tests.out

.PHONY: makeFolder

build: makeShared makeFolder $(OBJ)
	$(CC) main.c -o $(OUT_NAME) $(OBJ) $(SHARED) $(LINK) -g

./obj/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $< $(LINK) -g

makeShared:
	make -C ../shared

makeFolder:
	@if [ ! -d ./obj ]; then \
		mkdir obj; \
	fi

debug: CFLAGS := $(CFLAGS) -g
debug: build

valgrind: build
	valgrind --leak-check=full --track-origins=yes --log-file=valgrind.log ./$(OUT_NAME)

tests: build
	./$(OUT_NAME)

clean:
	rm -rf obj
	rm -rf *.out
	make clean -C ../shared