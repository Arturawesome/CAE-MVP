#!/usr/bin/env bash

set -e
BUILD_DIR="build"
EXECUTABLE="$BUILD_DIR/heatTask"

case "${1:-build}" in
    build)
        rm -rf build
        cmake -S . -B build
        cmake --build build
        ./build/heatTask
        ;;

    leaks)
        leaks -atExit -- ./build/heatTask
        ;;

    *)
        echo "Usage: $0 [build|leaks]"
        exit 1
        ;;
esac 