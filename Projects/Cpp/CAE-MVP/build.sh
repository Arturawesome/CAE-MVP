#!/usr/bin/env bash

set -e
BUILD_DIR="build"
EXECUTABLE="$BUILD_DIR/cae_mvp"

case "${1:-build}" in
    build)
        rm -rf build
        cmake -S . -B build
        cmake --build build
        ./build/cae_mvp
        ;;

    leaks)
        leaks -atExit -- ./build/cae_mvp
        ;;

    *)
        echo "Usage: $0 [build|leaks]"
        exit 1
        ;;
esac 