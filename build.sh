#!/usr/bin/env bash
# build.sh - Build DualShellCommander and its modules
# Implements the steps from README.md:
#  - build kernel module: modules/kernel (cmake build, make install)
#  - build user module:   modules/user   (cmake build, make install)
#  - build main project:  root            (cmake build, make)

set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$SCRIPT_DIR"
JOBS=1
DRY_RUN=0
VERBOSE=0
SKIP_KERNEL=0
SKIP_USER=0
SKIP_MAIN=0
CMAKE_CONFIGURE_ARGS=()
MAKE_TARGET=""
TOOLCHAIN_FILE=""
CMAKE_GENERATOR="Unix Makefiles"

usage() {
    cat <<EOF
Usage: $(basename "$0") [options]

Options:
  -j N        Parallel build jobs (default: number of processors)
  --dry-run   Print commands without running them
    --toolchain-file <path>  Path to vita.toolchain.cmake (overrides VITASDK)
    --cmake-arg <arg>        Extra argument to pass to cmake (can be repeated)
  --skip-kernel  Skip building modules/kernel
  --skip-user    Skip building modules/user
  --skip-main    Skip building main project
  -v           Verbose output
  -h, --help   Show this help

This script follows the README.md developer instructions:
  * Build modules/kernel: mkdir build && cd build && cmake .. && make install
  * Build modules/user:   mkdir build && cd build && cmake .. && make install
  * Build main project:   mkdir build && cd build && cmake .. && make

It expects vitasdk installed and environment variables (e.g., VITASDK) set when required by your environment.
EOF
}

# default jobs
if command -v nproc >/dev/null 2>&1; then
    JOBS=$(nproc)
else
    JOBS=1
fi

# parse args
while [[ $# -gt 0 ]]; do
    case "$1" in
        -j)
            JOBS="$2"
            shift 2
            ;;
        --dry-run)
            DRY_RUN=1
            shift
            ;;
        --skip-kernel)
            SKIP_KERNEL=1
            shift
            ;;
        --skip-user)
            SKIP_USER=1
            shift
            ;;
        --skip-main)
            SKIP_MAIN=1
            shift
            ;;
        --toolchain-file)
            TOOLCHAIN_FILE="$2"
            shift 2
            ;;
        --cmake-arg)
            CMAKE_CONFIGURE_ARGS+=("$2")
            shift 2
            ;;
        -v)
            VERBOSE=1
            shift
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        --)
            shift
            break
            ;;
        *)
            echo "Unknown option: $1" >&2
            usage
            exit 2
            ;;
    esac
done

run_cmd() {
    if [[ $DRY_RUN -eq 1 ]]; then
        echo "+ $*"
    else
        if [[ $VERBOSE -eq 1 ]]; then
            echo "+ $*"
        fi
        eval "$*"
    fi
}

check_requirements() {
    local missing=0
    for cmd in cmake make; do
        if ! command -v "$cmd" >/dev/null 2>&1; then
            echo "Error: required command '$cmd' not found in PATH." >&2
            missing=1
        fi
    done

    if [[ $missing -eq 1 ]]; then
        echo "Please install the missing tools and re-run." >&2
        exit 3
    fi

    if [[ -z "${VITASDK:-}" ]]; then
        echo "Warning: VITASDK environment variable is not set. vitasdk tools may not be available." >&2
        echo "If you have vitasdk installed, export VITASDK=/path/to/vitasdk before running, or pass --toolchain-file /path/to/vita.toolchain.cmake to this script." >&2
    fi
}

build_dir() {
    local target_dir="$1"
    local install_flag="${2:-}"

    echo "\n==> Building in: $target_dir"
    if [[ ! -d "$target_dir" ]]; then
        echo "Error: directory $target_dir does not exist" >&2
        exit 4
    fi

    pushd "$target_dir" >/dev/null
    run_cmd 'rm -rf build'
    run_cmd 'mkdir -p build'
    run_cmd 'cd build'
    # determine toolchain file argument for CMake
    local CMAKE_TOOLCHAIN_ARG=""
    if [[ -n "${TOOLCHAIN_FILE:-}" ]]; then
        if [[ -f "${TOOLCHAIN_FILE}" ]]; then
            CMAKE_TOOLCHAIN_ARG="-DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE}"
        else
            echo "Warning: provided toolchain file '${TOOLCHAIN_FILE}' not found." >&2
        fi
    else
        # try VITASDK-based defaults
        if [[ -n "${VITASDK:-}" ]]; then
            if [[ -f "$VITASDK/share/vita.toolchain.cmake" ]]; then
                CMAKE_TOOLCHAIN_ARG="-DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake"
            fi
        fi
        # common install location fallback
        if [[ -z "$CMAKE_TOOLCHAIN_ARG" && -f "/usr/local/vitasdk/share/vita.toolchain.cmake" ]]; then
            CMAKE_TOOLCHAIN_ARG="-DCMAKE_TOOLCHAIN_FILE=/usr/local/vitasdk/share/vita.toolchain.cmake"
        fi
    fi

    # build the cmake configure command
    local cmake_cmd="cmake -G \"$CMAKE_GENERATOR\" .."
    if [[ -n "$CMAKE_TOOLCHAIN_ARG" ]]; then
        cmake_cmd+=" $CMAKE_TOOLCHAIN_ARG"
    fi
    if [[ ${#CMAKE_CONFIGURE_ARGS[@]} -gt 0 ]]; then
        cmake_cmd+=" ${CMAKE_CONFIGURE_ARGS[*]}"
    fi
    run_cmd "$cmake_cmd"

    if [[ "$install_flag" == "install" ]]; then
        run_cmd "make -j $JOBS install"
    else
        run_cmd "make -j $JOBS ${MAKE_TARGET:-}"
    fi
    popd >/dev/null
}

# main
check_requirements

if [[ $SKIP_KERNEL -eq 0 ]]; then
    if [[ -d "$ROOT_DIR/modules/kernel" ]]; then
        build_dir "$ROOT_DIR/modules/kernel" install
    else
        echo "Note: modules/kernel directory not found, skipping kernel module." >&2
    fi
else
    echo "Skipping kernel build as requested."
fi

if [[ $SKIP_USER -eq 0 ]]; then
    if [[ -d "$ROOT_DIR/modules/user" ]]; then
        build_dir "$ROOT_DIR/modules/user" install
    else
        echo "Note: modules/user directory not found, skipping user module." >&2
    fi
else
    echo "Skipping user build as requested."
fi

if [[ $SKIP_MAIN -eq 0 ]]; then
    build_dir "$ROOT_DIR" ""
else
    echo "Skipping main build as requested."
fi

echo "\nBuild script finished."
