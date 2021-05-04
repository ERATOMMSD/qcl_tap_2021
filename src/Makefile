SRC=fault_tree_test.c fault_tree.c optimisation.c proof_test.c proof.c \
		logic_test.c logic.c sequent_test.c sequent.c formula_test.c formula.c \
		expression_test.c expression.c utils.c
MAIN=main.c
OBJ=$(SRC:.c=.o) $(MAIN:.c=.o)
HDR=$(SRC:.c=.h)
CC=gcc
LD=gcc
CFLAGS=-g -Wall -Wextra -Werror -pedantic -std=c99
LIBS=-lgc -lm -lcjson

all: main

main: $(OBJ) $(HDR)
	$(LD) $(OBJ) $(LIBS) -o main

%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f main *.o

fullclean: clean
	rm -f *~
