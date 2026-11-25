# Vendored Qt build

`third_party/qt6` is a minimal, headless build of Qt's `qtbase` used by
`./build.sh --standalone` (`-DCYRUS_STANDALONE=ON`). It is committed prebuilt, so
**building Cyrus never runs anything in this folder**. These files only exist to
regenerate `third_party/qt6`, e.g. to move to a new Qt version or enable a feature.

## What the build contains

- Modules: Core, Gui, Widgets, Sql (SQLite), Test, Network, Concurrent, Xml, PrintSupport
- Platform plugins: `offscreen`, `minimal`, `vnc` (no xcb/wayland, so no desktop windows)
- All third-party libraries (zlib, libpng, libjpeg, PCRE2, HarfBuzz, FreeType, SQLite, ...)
  are bundled; the result depends only on libc, libm, libgcc_s and libstdc++.
- No OpenGL, ICU, fontconfig, glib, dbus or OpenSSL.

It is built on Ubuntu 24.04 (GCC 13), so it runs there and on newer glibc-based distros.

## Files

- `Dockerfile` - the build environment (Ubuntu 24.04, GCC 13, CMake, Ninja).
- `build-qtbase.sh` - configures, builds and installs qtbase inside that container.
  The configure flags here define what the vendored Qt contains.

## Usage

Requires Docker and roughly 8 GB of RAM for the build; takes 30-60 minutes.

```sh
QT_VERSION=6.7.3
mkdir -p dl out

# 1. Download and verify the qtbase source
curl -L -o dl/qtbase-everywhere-src-$QT_VERSION.tar.xz \
  https://download.qt.io/archive/qt/${QT_VERSION%.*}/$QT_VERSION/submodules/qtbase-everywhere-src-$QT_VERSION.tar.xz
curl -sL https://download.qt.io/archive/qt/${QT_VERSION%.*}/$QT_VERSION/submodules/md5sums.txt \
  | grep "qtbase-everywhere-src-$QT_VERSION.tar.xz" | (cd dl && md5sum -c -)

# 2. Build the image and run the build (output lands in out/qt6)
docker build -t cyrus-qtbase-builder tools/qt
docker run --rm -u "$(id -u):$(id -g)" -e QT_VERSION=$QT_VERSION \
  -v "$PWD/dl:/dl:ro" -v "$PWD/out:/out" -v "$PWD/tools/qt:/scripts:ro" \
  --tmpfs /work:exec,size=8g \
  cyrus-qtbase-builder /scripts/build-qtbase.sh

# 3. Replace the vendored copy
rm -rf third_party/qt6 && cp -a out/qt6 third_party/qt6
```

Then check it with `./build.sh --standalone` and run `build-standalone/cyrus`;
it should start on the `offscreen` platform with no extra environment variables.
Remove `dl/` and `out/` afterwards.
