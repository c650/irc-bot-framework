CC= g++
CFLAGS= --std=c++17

LDLIBS= -lcurl
LDFLAGS= -L/usr/lib/x86_64-linux-gnu

SRC = src/bot.cpp src/googler.cpp src/http.cpp src/main.cpp src/packet.cpp src/server.cpp src/url-shortener.cpp
OUT = bin/bot.out

build: ${SRC}
	${CC} ${CFLAGS} -o ${OUT} ${SRC} ${LDFLAGS} ${LDLIBS}

.PHONY: clean

clean:
	rm ./bin/bot.out
