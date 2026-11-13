
#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${BUILD_DIR:-$ROOT_DIR/build}"

MODE="${1:-build}"
TARGET="${2:-all}"

configure() {
    cmake -S "$ROOT_DIR" -B "$BUILD_DIR" \
        -DCMAKE_BUILD_TYPE=Debug \
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="$ROOT_DIR"
}

build_target() {
    case "$1" in
        all)
            cmake --build "$BUILD_DIR" --parallel ;;
        heat)
            cmake --build "$BUILD_DIR" --target cae_heat_app --parallel ;;
        hydro)
            cmake --build "$BUILD_DIR" --target cae_hydro_app --parallel ;;
        deform)
            cmake --build "$BUILD_DIR" --target cae_deform_app --parallel ;;
        *)
            echo "Unknown target: $1" >&2
            exit 1 ;;
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

        cd "$ROOT_DIR"

        case "$TARGET" in
            heat) ./cae_heat ;;
            hydro) ./cae_hydro ;;
            deform) ./cae_deform ;;
            *)
                echo "Choose heat, hydro or deform" >&2
                exit 1 ;;
        esac
        ;;
    clean)
        cmake -E rm -rf "$BUILD_DIR"
        rm -rf "$ROOT_DIR/solutions" \
        rm -f "$ROOT_DIR/cae_heat" \
              "$ROOT_DIR/cae_hydro" \
              "$ROOT_DIR/cae_deform" 
        ;;
    *)
        echo "Usage: $0 {build|run|clean} [all|heat|hydro|deform]" >&2
        exit 1 ;;
esac
