#!/usr/bin/env bash

set -euo pipefail

echo "Note: This script shouldn't be ran as root, although a part of the script may require you to enter your password."
# Resolve the directory this script actually lives in.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

TOOLCHAIN_DIR="${TOOLCHAIN_DIR:-$SCRIPT_DIR/casio-toolchain-finish}"
PATCHES_DIR="${PATCHES_DIR:-$SCRIPT_DIR/patches}"
FAKE_HOME="$TOOLCHAIN_DIR"
GITEAPC_BIN="$FAKE_HOME/.local/bin/giteapc"

mkdir -p "$FAKE_HOME"

export HOME="$FAKE_HOME"
export PATH="$FAKE_HOME/.local/bin:$PATH"

log() {
    printf '\033[1;36m==> %s\033[0m\n' "$*"
}

warn() {
    printf '\033[1;33m!! %s\033[0m\n' "$*" >&2
}

run_yes() {
    local status

    set +o pipefail
    yes "y" | "$@"
    status="${PIPESTATUS[1]}"
    set -o pipefail

    return "$status"
}

check_deps() {
    local missing=()
    local cmd

    for cmd in curl git python3 make gcc pkg-config cmake; do
        command -v "$cmd" >/dev/null 2>&1 || missing+=("$cmd")
    done

    if [ "${#missing[@]}" -ne 0 ]; then
        warn "Missing host tools: ${missing[*]}"
        exit 1
    fi

    if [ "$(id -u)" -eq 0 ]; then
        warn "GiteaPC refuses to run as root."
        exit 1
    fi

    case "$(uname)" in
        Darwin)
            ;;
        *)
            warn "This script is intended for macOS."
            exit 1
            ;;
    esac
}

neutralize_conflicting_headers() {
    local INCLUDE_DIR="/usr/local/include"

    # These names can conflict with headers provided by the macOS SDK.
    #
    # For example, a third-party locale.h previously caused Clang to see
    # two definitions of struct lconv:
    #
    #   macOS SDK _locale.h
    #   /usr/local/include/locale.h
    #
    # We replace known conflicting top-level headers with empty,
    # root-owned read-only files before running GiteaPC. This prevents
    # stale third-party compatibility headers from interfering with SDK
    # headers while building the toolchain.

    local HEADERS=(
        alloca.h
        complex.h
        endian.h
        fenv.h
        locale.h
        stdint.h
        stdio.h
        syscall.h
    )

    log "Neutralizing conflicting headers in $INCLUDE_DIR"

    # Ask for sudo once here instead of repeatedly during the loop.
    sudo -v

    local header
    local path

    for header in "${HEADERS[@]}"; do
        path="$INCLUDE_DIR/$header"

        if [ -e "$path" ] || [ -L "$path" ]; then
            echo "    Removing: $path"
            sudo /bin/rm -rf "$path"
        fi
    done
}

install_giteapc() {
    if [ -x "$GITEAPC_BIN" ]; then
        log "GiteaPC already installed"
        return
    fi

    log "Please rename casio-toolchain-stub to casio-toolchain-finish first, it contains the modded GiteaPC package with patches."
    exit 1
}

install_toolchain() {
    log "Installing fxSDK dev:noudisks2 + gint dev with local patches"

    if [ ! -d "$PATCHES_DIR" ]; then
        warn "Patches dir not found at $PATCHES_DIR, continuing without patches"
    fi

    run_yes giteapc install \
        Lephenixnoir/fxsdk@dev:noudisks2 \
        Lephenixnoir/gint@dev \
        --patches "$PATCHES_DIR"
}

case "${1:-install}" in
    install)
        check_deps

        # Must happen before GiteaPC builds binutils/GCC.
        neutralize_conflicting_headers

        install_giteapc
        install_toolchain

        log "Toolchain installed in $TOOLCHAIN_DIR"
        ;;

    shell)
        export PATH="$FAKE_HOME/.local/bin:$PATH"
        exec "${SHELL:-bash}"
        ;;

    *)
        echo "Usage: $0 [install|shell]" >&2
        exit 1
        ;;
esac