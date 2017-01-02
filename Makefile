CC= g++
CFLAGS= --std=c++17

LDLIBS= -lcurl -lcrypto -lssl -lpthread
LDFLAGS= -L/usr/lib/x86_64-linux-gnu

SRC= src/IRCBot/*.cpp src/Plugins/*.cpp src/main.cpp
OUT= bin/bot.out

OPTIMIZE= -Os # reduce code size...

build: ${SRC}
	${CC} ${OPTIMIZE} ${CFLAGS} -o ${OUT} ${SRC} ${LDFLAGS} ${LDLIBS}

.PHONY: clean static

clean:
	rm ./bin/bot.out
