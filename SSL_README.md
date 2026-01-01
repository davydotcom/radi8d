# SSL/TLS Support for radi8d

## Overview
The radi8d server now supports SSL/TLS encrypted connections using OpenSSL.

## Features
- **Automatic Self-Signed Certificate Generation**: If no certificate is specified, the server automatically generates a self-signed certificate valid for 1 year
- **Custom Certificate Support**: Use your own SSL certificates with `--cert` and `--key` options
- **TLS 1.3 Support**: The implementation supports modern TLS 1.3 protocol
- **Backward Compatible**: Non-SSL mode still works without any changes

## Usage

### Basic SSL Mode (Self-Signed Certificate)
```bash
./radi8d -p 8443 --ssl
```
This will:
1. Generate a self-signed certificate (`radi8d_self_signed.crt`)
2. Generate a private key (`radi8d_self_signed.key`)
3. Start the server with SSL enabled on port 8443

The generated certificates are automatically cleaned up when the server shuts down gracefully.

### SSL Mode with Custom Certificate
```bash
./radi8d -p 8443 --ssl --cert /path/to/cert.pem --key /path/to/key.pem
```

### Non-SSL Mode (Default)
```bash
./radi8d -p 8080
```
The server works exactly as before without SSL.

## Command-Line Arguments

- `--ssl`: Enable SSL/TLS support
- `--cert <file>`: Path to SSL certificate file (PEM format)
- `--key <file>`: Path to SSL private key file (PEM format)

## Configuration File
You can also configure SSL in `radi8d.cfg`:

```ini
#SSL/TLS SETTINGS
SSL_ENABLED=OFF
SSL_CERT_PATH=
SSL_KEY_PATH=
```

## Testing SSL Connection
Test the SSL connection using OpenSSL client:
```bash
openssl s_client -connect localhost:8443 -brief
```

## Self-Signed Certificate Details
When auto-generated, the certificate contains:
- **Subject**: CN=localhost, O=radi8d, C=US
- **Validity**: 1 year from generation
- **Key Size**: 2048-bit RSA
- **Signature**: SHA256

## Requirements
- OpenSSL 3.x (via Homebrew on macOS: `brew install openssl@3`)
- The Makefile automatically links against OpenSSL libraries

## Implementation Details
- SSL context is managed in the `cswrapper` class
- Each client connection gets its own SSL session
- SSL handshake occurs immediately after accept()
- SSL sessions are properly cleaned up on disconnect

## Security Notes
- Self-signed certificates are suitable for development/testing only
- For production use, obtain a proper certificate from a Certificate Authority
- The server supports TLS 1.3 with modern cipher suites (e.g., TLS_AES_256_GCM_SHA384)

## Troubleshooting

### Certificate Verification Errors
If using self-signed certificates, clients may report verification errors. This is expected. Use `-k` or equivalent options in client tools to skip verification for testing.

### OpenSSL Not Found
If compilation fails with "openssl/ssl.h not found":
1. Install OpenSSL: `brew install openssl@3`
2. Update the `OPENSSL_PATH` in the Makefile if needed

### Port Already in Use
Ensure no other process is using the specified port:
```bash
lsof -i :8443
```
