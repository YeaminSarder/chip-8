

GCC = g++ -std=c++2a
OBJ = chip8.cpp funcs.o term.o
main: ${OBJ}
	${GCC} -pthread -o chip8 ${OBJ}
funcs.o: funcs.cpp funcs.h config.h
	${GCC} -c -o funcs.o funcs.cpp
term.o: term.cpp term.h
	${GCC} -c -o term.o term.cpp
