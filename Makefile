CC= g++
CFLAGS= --std=c++17 -rdynamic

LDLIBS= -lcurl -lcrypto -lssl -lpthread -ldl
LDFLAGS= -L/usr/lib/x86_64-linux-gnu

SRC= src/IRCBot/*.cpp src/main.cpp
OUT= bin/bot.out

OPTIMIZE= -Os # reduce code size...

build: ${SRC}
	${CC} ${OPTIMIZE} ${CFLAGS} -o ${OUT} ${SRC} ${LDFLAGS} ${LDLIBS}

.PHONY: clean static

clean:
	rm ./bin/bot.out

# for dynamic plugins:
# g++ --std=c++17 -Os -fPIC -rdynamic -shared -o ./bin/iplookup.so ./src/Plugins/http.cpp ./src/Plugins/iplookup.cpp -lcurl
# obviously don't link curl or compile wih unnecessary files...
