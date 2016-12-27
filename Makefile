CC= g++
CFLAGS= --std=c++17

LDLIBS= -lcurl -lcrypto -lssl
LDFLAGS= -L/usr/lib/x86_64-linux-gnu

SRC = src/*.cpp
OUT = bin/bot.out

build: ${SRC}
	${CC} ${CFLAGS} -o ${OUT} ${SRC} ${LDFLAGS} ${LDLIBS}

.PHONY: clean

clean:
	rm ./bin/bot.out
