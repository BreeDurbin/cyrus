# Build environment for the vendored headless Qt: Ubuntu 24.04, GCC 13, CMake 3.28.
FROM ubuntu:24.04
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
      build-essential cmake ninja-build xz-utils ca-certificates \
    && rm -rf /var/lib/apt/lists/*
