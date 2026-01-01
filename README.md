# radi8d - Radio Server Daemon

A lightweight, high-performance C++ server daemon with SSL/TLS support for handling multiple client connections.

## Features

- **Multi-client Support** - Handle thousands of simultaneous connections using select-based event loop
- **Channel System** - Support for multiple channels with user management
- **SSL/TLS Encryption** - Optional SSL/TLS support with automatic self-signed certificate generation
- **WebSocket Support** - Built-in WebSocket protocol handling
- **User Management** - User authentication and profile management
- **File Sharing** - Support for file transfers between clients
- **Configurable** - Easy configuration via `radi8d.cfg` file

## Requirements

- C++ compiler (g++ or clang++)
- OpenSSL 3.x (for SSL support)
- POSIX-compliant operating system (Linux, macOS, BSD)

### macOS Installation
```bash
brew install openssl@3
```

## Building

```bash
make compile
```

To clean the build:
```bash
make clean
```

## Usage

### Basic Usage (Non-SSL)
```bash
./radi8d -p 1337
```

### SSL Mode with Auto-Generated Certificate
```bash
./radi8d -p 1337 --ssl
```
This automatically generates a self-signed certificate for testing/development.

### SSL Mode with Custom Certificate
```bash
./radi8d -p 1337 --ssl --cert /path/to/cert.pem --key /path/to/key.pem
```

### Daemon Mode
```bash
./radi8d -p 1337 -d
```

### Debug Mode
```bash
./radi8d -p 1337 --debug
```

### Full Example
```bash
./radi8d -p 1337 --ssl --debug --log
```

## Command-Line Arguments

| Argument | Description |
|----------|-------------|
| `-p <port>` | **Required.** Specify the port to listen on (default: 1337) |
| `--ssl` | Enable SSL/TLS encryption |
| `--cert <file>` | Path to SSL certificate file (PEM format) |
| `--key <file>` | Path to SSL private key file (PEM format) |
| `-d` | Run in daemon mode (background process) |
| `--debug` | Enable debug output |
| `--log` | Enable logging to file |
| `--nodebug` | Explicitly disable debug mode |

## Configuration

Edit `radi8d.cfg` to configure server settings:

```ini
#GENERAL
DEBUG=ON
DAEMONMODE=OFF
ERRORFILE=radi8d.err
THREADED=OFF

#SSL/TLS SETTINGS
SSL_ENABLED=OFF
SSL_CERT_PATH=
SSL_KEY_PATH=

#USER VARS
USERFILE=radi8d.usr
PROFILELOCATION=profiles
MAXUSERS=500
MAXFILESHARES=500
MAXPROFILESIZE=1024
MAXFILETRANSFERRATE=580

#CHANVARS
MAXCHANNELS=8000
MAXCHANUSERS=1000
DEFAULTCODEC=2

#DEFAULT CHANNEL SETTINGS
DEFPASSWORD=
DEFCHANTYPE=0
DEFCODEC=1
DEFRSPEAK=0
DEFRESERVED=0
DEFTOPIC=radi8d 2.0 Testing Server

#TEMPORARY VARS FOR TESTING
PASSWORD=RANDOM
```

## SSL/TLS Support

For detailed SSL/TLS documentation, see [SSL_README.md](SSL_README.md).

### Quick SSL Setup
1. **Auto-generate certificate** (development/testing):
   ```bash
   ./radi8d -p 1337 --ssl
   ```

2. **Use custom certificate** (production):
   ```bash
   ./radi8d -p 1337 --ssl --cert server.crt --key server.key
   ```

3. **Test SSL connection**:
   ```bash
   openssl s_client -connect localhost:1337 -brief
   ```

## Project Structure

```
radi8d/
├── src/                    # Source files
│   ├── main.cpp           # Main entry point
│   ├── cSCore.cpp         # Core server logic
│   ├── cUserHandler.cpp   # User management
│   ├── cChannelHandler.cpp # Channel management
│   ├── getconfig.cpp      # Configuration parser
│   └── ...
├── include/               # Header files
│   ├── cSCore.h
│   ├── cUserHandler.h
│   ├── cChannelHandler.h
│   └── ...
├── swrapper/             # Socket wrapper library
│   ├── src/
│   └── include/
├── profiles/             # User profiles directory
├── radi8d.cfg           # Configuration file
├── Makefile             # Build configuration
└── README.md            # This file
```

## Development

### Adding Features
The modular design allows easy extension:
- **User handling**: Extend `cUserHandler` class
- **Channel features**: Modify `cChannelHandler` class
- **Protocol changes**: Update `cSCore::parsecmd()` method
- **Socket operations**: Extend `cswrapper` class

### Debugging
Enable debug mode to see detailed server operations:
```bash
./radi8d -p 1337 --debug
```

Debug output is written to the error file specified in `radi8d.cfg` (default: `radi8d.err`).

## Architecture

- **Event-Driven**: Uses `select()` for efficient I/O multiplexing
- **Non-Blocking**: Handles multiple clients without threading overhead
- **Modular**: Separate handlers for users, channels, and protocols
- **Secure**: SSL/TLS support with modern cipher suites

## Testing

### Basic Connection Test
```bash
# Start server
./radi8d -p 1337

# In another terminal, connect with telnet
telnet localhost 1337
```

### SSL Connection Test
```bash
# Start server with SSL
./radi8d -p 1337 --ssl

# In another terminal, test with OpenSSL
openssl s_client -connect localhost:1337
```

## Troubleshooting

### Port Already in Use
```bash
# Find process using the port
lsof -i :1337

# Kill the process if needed
kill -9 <PID>
```

### OpenSSL Errors
If you get SSL-related compilation errors:
1. Ensure OpenSSL is installed: `brew install openssl@3`
2. Update `OPENSSL_PATH` in Makefile if needed
3. Run `make clean && make compile`

### Permission Denied
If running on privileged ports (< 1024), use sudo:
```bash
sudo ./radi8d -p 443 --ssl
```

## License

GPL License - For non-commercial use only.

Copyright 2004-2026 David Estes

## Version History

- **v2.0** - Added SSL/TLS support with auto-generated certificates
- **v1.2** - WebSocket protocol support
- **v1.0** - Initial release

## Author

David Estes (Chrono)

## Contributing

This is a personal/educational project. Feel free to fork and modify for your own use.
