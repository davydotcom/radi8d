#!/bin/bash
OPENSSL_PATH="/opt/homebrew/opt/openssl@3"
CFLAGS="-c -Iinclude -I${OPENSSL_PATH}/include"
if [ "$1" == "unix" ]
then
CFLAGS="$CFLAGS -lsocket"
fi
echo "Compiling swrapper..."
g++ $CFLAGS src/swrapper.c
if [ "$2" == "c++" ]
then
echo "Compiling cswrapper (Class Interface)"
g++ $CFLAGS src/cswrapper.cpp
fi
echo "Done!"
