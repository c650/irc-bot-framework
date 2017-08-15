# This Makefile compiles the IRCBot code into a shared object file for linking into
# an executable that makes use of the bot's code.

CC= g++
CFLAGS= --std=c++17 -shared -fPIC



SRC= src/IRCBot/*.cpp
OUT= bin/libbot.a

OPTIMIZE= -Os # reduce code size...

build: ${SRC}
	${CC} ${OPTIMIZE} ${CFLAGS} -c ${SRC}
	ar -cvq ${OUT} *.o
	rm *.o

.PHONY: clean static

clean:
	rm ${OUT}

# ${CC} -o ${OUT} *.o ${LDFLAGS} ${LDLIBS} -shared
