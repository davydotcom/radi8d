# Multi-stage build for radi8d server
FROM ubuntu:22.04 AS builder

# Install build dependencies
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /build

# Copy source files
COPY src/ ./src/
COPY include/ ./include/
COPY swrapper/ ./swrapper/
COPY hash_password.cpp .
COPY Makefile.docker ./Makefile

# Clean any stale object files and build
RUN find . -name '*.o' -delete && make compile && make utils

# Runtime stage
FROM ubuntu:22.04

# Install runtime dependencies
RUN apt-get update && apt-get install -y \
    libssl3 \
    && rm -rf /var/lib/apt/lists/*

# Create application directory
WORKDIR /app

# Copy built binaries from builder
COPY --from=builder /build/radi8d /app/
COPY --from=builder /build/hash_password /app/

# Copy default configuration files
COPY radi8d.cfg /app/radi8d.cfg.default
COPY radi8d.motd /app/radi8d.motd.default
COPY radi8d.usr /app/radi8d.usr.default

# Create profiles directory
RUN mkdir -p /app/profiles

# Environment variables for configuration
ENV PORT=1337
ENV SSL_ENABLED=false
ENV SSL_CERT_PATH=""
ENV SSL_KEY_PATH=""
ENV DEBUG=false
ENV DAEMON=false

# Expose default port (can be overridden)
EXPOSE 1337

# Copy entrypoint script
COPY docker-entrypoint.sh /app/
RUN chmod +x /app/docker-entrypoint.sh

ENTRYPOINT ["/app/docker-entrypoint.sh"]
