SRC_DIR = ./src
_SRC = $(shell find ./src -type f -iname "*.c" -exec basename {} \;)
SRC = $(patsubst %,$(SRC_DIR)/%,$(SRC))

ODIR = ./obj
_OBJ = $(_SRC:.c=.o)
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

HEADERS = $(shell find ./include/*.h) $(shell find ../mi-ram-hq/include/*.h)

MIRAMHQ = $(shell find ../mi-ram-hq/obj/*.o)
SHARED = $(shell find ../shared/obj/*.o)

O_DIR = ./obj
CC = gcc

CFLAGS = -c

LINK = -lcommons -lreadline -lpthread -lnivel-gui -lncurses -lcunit

OUT_NAME = tests.out

.PHONY: makeFolder

build: makeShared makeFolder makeMRH $(OBJ)
	$(CC) main.c -o $(OUT_NAME) $(OBJ) $(SHARED) $(MIRAMHQ) $(LINK) -g

./obj/%.o: $(SRC_DIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $< $(LINK) -g

makeShared:
	make -C ../shared

makeMRH:
	make -C ../mi-ram-hq

makeFolder:
	@if [ ! -d ./obj ]; then \
		mkdir obj; \
	fi

debug: CFLAGS := $(CFLAGS) -g
debug:
	make debug -C ../shared
	make debug -C ../mi-ram-hq
	make build

valgrind: debug
	valgrind --leak-check=full --track-origins=yes --log-file=valgrind.log ./$(OUT_NAME)

tests: build
	./$(OUT_NAME)

clean:
	rm -rf obj
	rm -rf *.out
	make clean -C ../shared
	make clean -C ../mi-ram-hq