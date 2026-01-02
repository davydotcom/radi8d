#Makefile radi8d
.SUFFIXES:
.SUFFIXES: .cpp $(SUFFIXES)
CC = g++

# Detect operating system
UNAME_S := $(shell uname -s)

# Set OpenSSL path based on OS
ifeq ($(UNAME_S),Darwin)
	# macOS with Homebrew
	ifeq ($(shell uname -m),arm64)
		OPENSSL_PATH = /opt/homebrew/opt/openssl@3
	else
		OPENSSL_PATH = /usr/local/opt/openssl@3
	endif
	OPENSSL_INCLUDE = -I$(OPENSSL_PATH)/include
	OPENSSL_LIB = -L$(OPENSSL_PATH)/lib
else
	# Linux (Ubuntu/Debian)
	OPENSSL_INCLUDE = 
	OPENSSL_LIB = 
endif

CFLAGS=-Wall -Iinclude -Iswrapper/include $(OPENSSL_INCLUDE) -c -g
LFLAGS = -Wall -Iinclude -Iswrapper/include $(OPENSSL_INCLUDE) $(OPENSSL_LIB) -lssl -lcrypto -g
OBJS = swrapper/swrapper.o swrapper/cswrapper.o errhandle.o cChannelHandler.o cUserHandler.o cSCore.o cSCore-2.o getconfig.o getarg.o
VERSION = 1.2
compile: $(OBJS)
	$(CC) src/main.cpp $(LFLAGS) $(OBJS) -o radi8d

utils: hash_password

hash_password:
	$(CC) hash_password.cpp $(OPENSSL_INCLUDE) $(OPENSSL_LIB) -lssl -lcrypto -o hash_password

errhandle.o:
	$(CC) $(CFLAGS) src/errhandle.cpp
cChannelHandler.o:
	$(CC) $(CFLAGS) src/cChannelHandler.cpp
cUserHandler.o:
	$(CC) $(CFLAGS) src/cUserHandler.cpp
cSCore.o:
	$(CC) $(CFLAGS) src/cSCore.cpp
cSCore-2.o:
	$(CC) $(CFLAGS) src/cSCore-2.cpp
swrapper/swrapper.o: 
	cd swrapper ; ./compile.sh linux c++ ; cd ..
swrapper/cswrapper.o:
	cd swrapper ; ./compile.sh linux c++ ; cd ..
getarg.o:
	$(CC) $(CFLAGS) src/getarg.cpp
getconfig.o:
	$(CC) $(CFLAGS) src/getconfig.cpp
clean:
	\rm *.o
	\rm swrapper/*.o
	\rm radi8d
tar:
	mkdir radi8d-${VERSION}
	cp -r server/* radi8d-${VERSION}/
	tar cfv radi8d-${VERSION}.tar radi8d-${VERSION}
	gzip radi8d-${VERSION}.tar
	rm -fr radi8d-${VERSION}
	cd server
install:
	cp radi8d /usr/bin
#end Makefile

