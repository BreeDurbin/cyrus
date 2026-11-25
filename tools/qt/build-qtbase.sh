#!/bin/sh
# Minimal qtbase: Core/Gui/Widgets/Sql/Test, offscreen/minimal/vnc platforms,
# every third-party lib bundled, so it only needs libc/libstdc++/libm.
# Expects the qtbase source tarball in /dl and installs to /out/qt6.
set -eu
QT_VERSION=${QT_VERSION:-6.7.3}

mkdir -p /work && cd /work
tar xf /dl/qtbase-everywhere-src-${QT_VERSION}.tar.xz
mkdir build && cd build
../qtbase-everywhere-src-${QT_VERSION}/configure \
  -prefix /out/qt6 -release -shared -opensource -confirm-license \
  -nomake examples -nomake tests \
  -no-opengl -no-xcb -no-xkbcommon -no-glib -no-dbus -no-icu -no-fontconfig \
  -no-openssl -no-zstd -no-libudev -no-evdev -no-libinput -no-mtdev -no-tslib \
  -no-eglfs -no-linuxfb -no-vulkan -no-gbm -no-kms -no-cups -no-gtk \
  -qt-zlib -qt-libpng -qt-libjpeg -qt-pcre -qt-harfbuzz -qt-freetype \
  -sql-sqlite -qt-sqlite \
  -- -DFEATURE_system_doubleconversion=OFF -DFEATURE_system_libb2=OFF \
     -DFEATURE_system_textmarkdownreader=OFF -DFEATURE_brotli=OFF \
     -DFEATURE_gssapi=OFF -DFEATURE_libproxy=OFF -DFEATURE_sql_odbc=OFF \
     -DFEATURE_sql_psql=OFF -DFEATURE_sql_mysql=OFF
cmake --build . --parallel "$(nproc)"
cmake --install .
