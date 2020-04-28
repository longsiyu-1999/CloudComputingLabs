CXXFLAGS+=-O2 -ggdb -DDEBUG
CXXFLAGS+=-Wall -Wextra

all: httpserver

httpserver: httpserver.cc
	g++ -O2 -o $@ $^ -pthread

clean:
	rm httpserver
