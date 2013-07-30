# "make" to build a shared library
# "make static" to build a static library
# "make examples" to build all the examples (against a statically linked object
# file)

#Modify these if necessary:
GCC=clang
RM=rm -f
FLAGS=-Os
#FLAGS=-O0 -ggdb -Wall -Wextra
LIBCURSES=-lncursesw
##########################################################################

all: shared

obj:
	$(GCC) -c -fpic $(FLAGS) clicards.c -o clicards.o

shared: obj
	$(GCC) -shared -Wl,-soname,clicards.so.1 -o clicards.so.1 clicards.o

static: obj
	ar rcs libclicards.a clicards.o

examples: obj
	$(GCC) -c examples/calculation.c -o examples/calculation.o -I. $(FLAGS)
	$(GCC) -o examples/calculation examples/calculation.o clicards.o $(FLAGS) $(LIBCURSES)
	$(GCC) -c examples/poker.c -o examples/poker.o -I. $(FLAGS)
	$(GCC) -o examples/poker examples/poker.o clicards.o $(FLAGS) $(LIBCURSES)
	$(GCC) -c examples/test_curses.c -o examples/test_curses.o -I. $(FLAGS)
	$(GCC) -o examples/test_curses examples/test_curses.o clicards.o $(FLAGS) $(LIBCURSES)

clean:
	$(RM) clicards.o libclicards.a clicards.so.1 examples/*.o \
	examples/calculation examples/poker examples/test_curses

