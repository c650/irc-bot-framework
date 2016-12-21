CC= g++
CFLAGS= --std=c++17

SRC = src/*.cpp
OUT = bin/bot.out

build: ${SRC}
	${CC} ${CFLAGS} -o ${OUT} ${SRC}

.PHONY: clean

clean:
	rm ./bin/bot.out
