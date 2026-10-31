#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build}"
MODE="${1:-build}"
TARGET="${2:-all}"

configure() {
    cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug
}

build_target() {
    case "$1" in
        all) cmake --build "$BUILD_DIR" --parallel ;;
        heat) cmake --build "$BUILD_DIR" --target cae_heat_app --parallel ;;
        hydro) cmake --build "$BUILD_DIR" --target cae_hydro_app --parallel ;;
        *) echo "Unknown target: $1" >&2; exit 1 ;;
    esac
}

case "$MODE" in
    build)
        configure
        build_target "$TARGET"
        ;;
    run)
        configure
        build_target "$TARGET"
        case "$TARGET" in
            heat) "./$BUILD_DIR/cae_heat" ;;
            hydro) "./$BUILD_DIR/cae_hydro" ;;
            *) echo "For run choose heat or hydro" >&2; exit 1 ;;
        esac
        ;;
    clean)
        cmake -E rm -rf "$BUILD_DIR"
        ;;
    leaks)
        case "$TARGET" in
            heat) leaks -atExit -- "./$BUILD_DIR/cae_heat" ;;
            hydro) leaks -atExit -- "./$BUILD_DIR/cae_hydro" ;;
            *) echo "For leaks choose heat or hydro" >&2; exit 1 ;;
        esac
        ;;
    *)
        echo "Usage: $0 {build|run|clean|leaks} [all|heat|hydro]" >&2
        exit 1
        ;;
esac
