#!/bin/sh
# Generate and build Cyrus.
#   ./build.sh               system Qt, output in build/
#   ./build.sh --standalone  headless Qt from third_party/qt6, output in build-standalone/
# Any other arguments are passed to the cmake generation step.
set -eu

cd "$(dirname "$0")"

build_dir=build
standalone=OFF
for arg in "$@"; do
    case "$arg" in
        --standalone) standalone=ON; build_dir=build-standalone ;;
        -h|--help) sed -n '2,5p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
    esac
done

# Drop --standalone, keep the rest for cmake.
for arg in "$@"; do
    shift
    [ "$arg" = "--standalone" ] || set -- "$@" "$arg"
done

cmake -S . -B "$build_dir" -DCYRUS_STANDALONE="$standalone" "$@"
cmake --build "$build_dir" --parallel "$(nproc)"
