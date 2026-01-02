# Docker Deployment Guide for radi8d

This guide shows you how to run radi8d in a Docker container with custom configuration.

## Quick Start

### Basic Usage (Default Settings)
```bash
docker build -t radi8d .
docker run -p 1337:1337 radi8d
```

### With Custom Port
```bash
docker run -p 8080:8080 -e PORT=8080 radi8d
```

## Custom Configuration

### Using Custom MOTD and User Files

Create your custom files on your host:
```bash
# Custom MOTD
cat > my-custom.motd << 'EOF'
Welcome to My Custom radi8d Server!
===================================
Server Rules:
1. Be respectful
2. No spam
3. Have fun!
EOF

# Custom users (generate password hash first)
docker run --rm radi8d /app/hash_password "mypassword"
# Copy the hash output, then create user file
cat > my-users.usr << 'EOF'
admin:YOUR_HASH_HERE:999:1
user1:ANOTHER_HASH_HERE:100:1
EOF
```

Run with custom files:
```bash
docker run -p 1337:1337 \
  -v $(pwd)/my-custom.motd:/app/radi8d.motd \
  -v $(pwd)/my-users.usr:/app/radi8d.usr \
  radi8d
```

### Using Custom Configuration File

Create a custom `radi8d.cfg`:
```bash
cat > my-config.cfg << 'EOF'
#GENERAL
DEBUG=ON
DAEMONMODE=OFF
ERRORFILE=radi8d.err
THREADED=OFF

#USER VARS
USERFILE=radi8d.usr
PROFILELOCATION=profiles
MAXUSERS=1000
MAXFILESHARES=500
MAXPROFILESIZE=1024
MAXFILETRANSFERRATE=580

#CHANVARS
MAXCHANNELS=10000
MAXCHANUSERS=2000
DEFAULTCODEC=2
EOF
```

Run with custom config:
```bash
docker run -p 1337:1337 \
  -v $(pwd)/my-config.cfg:/app/radi8d.cfg \
  radi8d
```

## SSL/TLS Configuration

### Auto-Generated Self-Signed Certificate
```bash
docker run -p 1337:1337 \
  -e SSL_ENABLED=true \
  radi8d
```

### Custom SSL Certificates

Place your certificate files on your host:
```bash
# Your existing certificates
# - server.crt (certificate)
# - server.key (private key)
```

Run with custom SSL certificates:
```bash
docker run -p 1337:1337 \
  -e SSL_ENABLED=true \
  -e SSL_CERT_PATH=/app/certs/server.crt \
  -e SSL_KEY_PATH=/app/certs/server.key \
  -v $(pwd)/server.crt:/app/certs/server.crt:ro \
  -v $(pwd)/server.key:/app/certs/server.key:ro \
  radi8d
```

## Environment Variables

| Variable | Default | Description |
|----------|---------|-------------|
| `PORT` | `1337` | Port the server listens on |
| `SSL_ENABLED` | `false` | Enable SSL/TLS (`true` or `false`) |
| `SSL_CERT_PATH` | `""` | Path to SSL certificate file (inside container) |
| `SSL_KEY_PATH` | `""` | Path to SSL private key file (inside container) |
| `DEBUG` | `false` | Enable debug mode (`true` or `false`) |
| `DAEMON` | `false` | Run in daemon mode (`true` or `false`) |

## Complete Example with All Options

```bash
docker run -d \
  --name my-radi8d-server \
  -p 8443:8443 \
  -e PORT=8443 \
  -e SSL_ENABLED=true \
  -e SSL_CERT_PATH=/app/certs/server.crt \
  -e SSL_KEY_PATH=/app/certs/server.key \
  -e DEBUG=true \
  -v $(pwd)/my-custom.motd:/app/radi8d.motd \
  -v $(pwd)/my-users.usr:/app/radi8d.usr \
  -v $(pwd)/my-config.cfg:/app/radi8d.cfg \
  -v $(pwd)/server.crt:/app/certs/server.crt:ro \
  -v $(pwd)/server.key:/app/certs/server.key:ro \
  -v $(pwd)/profiles:/app/profiles \
  --restart unless-stopped \
  radi8d
```

## Volume Mounts

Persist data across container restarts by mounting volumes:

```bash
docker run -d \
  -p 1337:1337 \
  -v radi8d-profiles:/app/profiles \
  -v $(pwd)/radi8d.motd:/app/radi8d.motd \
  -v $(pwd)/radi8d.usr:/app/radi8d.usr \
  radi8d
```

### Important Paths

| Container Path | Purpose |
|----------------|---------|
| `/app/radi8d.motd` | Message of the day shown to clients |
| `/app/radi8d.usr` | User authentication database |
| `/app/radi8d.cfg` | Server configuration |
| `/app/profiles` | User profile storage |
| `/app/certs/` | SSL certificate location (custom path) |

## Docker Compose

Create `docker-compose.yml`:

```yaml
version: '3.8'

services:
  radi8d:
    build: .
    image: radi8d:latest
    container_name: radi8d-server
    ports:
      - "1337:1337"
    environment:
      - PORT=1337
      - SSL_ENABLED=false
      - DEBUG=false
    volumes:
      - ./radi8d.motd:/app/radi8d.motd
      - ./radi8d.usr:/app/radi8d.usr
      - ./radi8d.cfg:/app/radi8d.cfg
      - profiles:/app/profiles
    restart: unless-stopped

volumes:
  profiles:
```

With SSL:

```yaml
version: '3.8'

services:
  radi8d:
    build: .
    image: radi8d:latest
    container_name: radi8d-server
    ports:
      - "1337:1337"
    environment:
      - PORT=1337
      - SSL_ENABLED=true
      - SSL_CERT_PATH=/app/certs/server.crt
      - SSL_KEY_PATH=/app/certs/server.key
      - DEBUG=false
    volumes:
      - ./radi8d.motd:/app/radi8d.motd
      - ./radi8d.usr:/app/radi8d.usr
      - ./radi8d.cfg:/app/radi8d.cfg
      - ./server.crt:/app/certs/server.crt:ro
      - ./server.key:/app/certs/server.key:ro
      - profiles:/app/profiles
    restart: unless-stopped

volumes:
  profiles:
```

Run with docker-compose:
```bash
docker-compose up -d
```

## Utility Commands

### Generate Password Hash
```bash
docker run --rm radi8d /app/hash_password "your_password_here"
```

### View Logs
```bash
docker logs -f my-radi8d-server
```

### Access Container Shell
```bash
docker exec -it my-radi8d-server /bin/bash
```

### Stop Server
```bash
docker stop my-radi8d-server
```

### Remove Container
```bash
docker rm my-radi8d-server
```

## Testing the Server

### Test Basic Connection
```bash
telnet localhost 1337
```

### Test SSL Connection
```bash
openssl s_client -connect localhost:1337 -brief
```

### Test from Another Container
```bash
docker run -it --rm --link my-radi8d-server:radi8d ubuntu:22.04 bash
# Inside container:
apt-get update && apt-get install -y telnet
telnet radi8d 1337
```

## Production Deployment Tips

1. **Use a reverse proxy** (nginx/traefik) for SSL termination
2. **Set resource limits**:
   ```bash
   docker run --memory="512m" --cpus="1.0" ...
   ```
3. **Use Docker secrets** for sensitive files
4. **Enable logging**:
   ```bash
   docker run --log-driver json-file --log-opt max-size=10m ...
   ```
5. **Use health checks** in docker-compose:
   ```yaml
   healthcheck:
     test: ["CMD", "nc", "-z", "localhost", "1337"]
     interval: 30s
     timeout: 10s
     retries: 3
   ```

## Troubleshooting

### Port Already in Use
```bash
# Find what's using the port
lsof -i :1337
# Or use a different port
docker run -p 2000:1337 radi8d
```

### Permission Issues with Volume Mounts
```bash
# Ensure files are readable
chmod 644 radi8d.motd radi8d.usr radi8d.cfg
# Ensure profile directory is writable
chmod 755 profiles
```

### SSL Certificate Issues
```bash
# Verify certificates are mounted correctly
docker exec my-radi8d-server ls -la /app/certs/

# Check certificate validity
docker exec my-radi8d-server openssl x509 -in /app/certs/server.crt -text -noout
```

### Build Issues
```bash
# Clean build
docker build --no-cache -t radi8d .

# Check build logs
docker build -t radi8d . 2>&1 | tee build.log
```
