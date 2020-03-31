CXXFLAGS+=-O2 -ggdb -DDEBUG
CXXFLAGS+=-Wall -Wextra

all: sudoku_solve

sudoku_solve: main.cc sudoku_basic.cc
	g++ -O2 -o $@ $^ -lpthread

clean:
	rm sudoku_solve
