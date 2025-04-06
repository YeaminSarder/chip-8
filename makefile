

GCC = g++ -std=c++2a
OBJ = chip8.o funcs.o term.o options.o interpreter_rom.o
chip8: ${OBJ}
	${GCC} -pthread -o chip8 ${OBJ}
chip8.o: chip8.cpp
	${GCC} -c -o chip8.o chip8.cpp
funcs.o: funcs.cpp funcs.h config.h
	${GCC} -c -o funcs.o funcs.cpp
term.o: term.cpp term.h
	${GCC} -c -o term.o term.cpp
options.o: options.cpp options.h
	${GCC} -c -o options.o options.cpp
interpreter_rom.o: interpreter_rom.rom
	ld -o interpreter_rom.o --format binary -r interpreter_rom.rom
