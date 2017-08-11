# This Makefile compiles the IRCBot code into a shared object file for linking into
# an executable that makes use of the bot's code.

CC= g++
CFLAGS= --std=c++17 -rdynamic -fPIC -shared

LDLIBS= -lcurl -lcrypto -lssl -lpthread -ldl
LDFLAGS= -L/usr/lib/x86_64-linux-gnu

SRC= src/IRCBot/*.cpp
OUT= bin/libbot.so

OPTIMIZE= -Os # reduce code size...

build: ${SRC}
	${CC} ${OPTIMIZE} ${CFLAGS} -o ${OUT} ${SRC} ${LDFLAGS} ${LDLIBS}

.PHONY: clean static

clean:
	rm ${OUT}
