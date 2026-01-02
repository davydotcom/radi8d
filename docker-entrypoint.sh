#!/bin/bash
set -e

# Use custom MOTD if provided via volume mount, otherwise use default
if [ ! -f /app/radi8d.motd ]; then
    cp /app/radi8d.motd.default /app/radi8d.motd
fi

# Use custom user file if provided via volume mount, otherwise use default
if [ ! -f /app/radi8d.usr ]; then
    cp /app/radi8d.usr.default /app/radi8d.usr
fi

# Use custom config if provided via volume mount, otherwise use default
if [ ! -f /app/radi8d.cfg ]; then
    cp /app/radi8d.cfg.default /app/radi8d.cfg
fi

# Build command line arguments
CMD_ARGS="-p ${PORT}"

if [ "$SSL_ENABLED" = "true" ]; then
    CMD_ARGS="$CMD_ARGS --ssl"
    
    if [ -n "$SSL_CERT_PATH" ] && [ -f "$SSL_CERT_PATH" ]; then
        CMD_ARGS="$CMD_ARGS --cert $SSL_CERT_PATH"
    fi
    
    if [ -n "$SSL_KEY_PATH" ] && [ -f "$SSL_KEY_PATH" ]; then
        CMD_ARGS="$CMD_ARGS --key $SSL_KEY_PATH"
    fi
fi

if [ "$DEBUG" = "true" ]; then
    CMD_ARGS="$CMD_ARGS --debug"
fi

if [ "$DAEMON" = "true" ]; then
    CMD_ARGS="$CMD_ARGS -d"
fi

# Log startup information
echo "Starting radi8d server..."
echo "Port: $PORT"
echo "SSL Enabled: $SSL_ENABLED"
echo "Debug: $DEBUG"
echo "Command: /app/radi8d $CMD_ARGS"

# Execute the server
exec /app/radi8d $CMD_ARGS
