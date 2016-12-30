CC= g++
CFLAGS= --std=c++17

LDLIBS= -lcurl -lcrypto -lssl
LDFLAGS= -L/usr/lib/x86_64-linux-gnu

SRC = src/IRCBot/*.cpp src/Plugins/*.cpp src/main.cpp
OUT = bin/bot.out

build: ${SRC}
	${CC} ${CFLAGS} -o ${OUT} ${SRC} ${LDFLAGS} ${LDLIBS}

.PHONY: clean static

# static: ${SRC}
# 	${CC} -static ${CFLAGS} -o ${OUT}.static ${SRC} ${LDFLAGS} ${LDLIBS}

clean:
	rm ./bin/bot.out
