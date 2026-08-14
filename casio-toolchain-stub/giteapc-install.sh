#!/usr/bin/env bash

set -euo pipefail

TAG=$(printf "\x1b[36m<giteapc>\x1b[0m")
URL="https://git.planet-casio.com/Lephenixnoir/GiteaPC"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Make the toolchain stub act as HOME.
export HOME="$SCRIPT_DIR"

export GITEAPC_PREFIX="$HOME/.local"
PREFIX="$GITEAPC_PREFIX"

export PATH="$PREFIX/bin:$PATH"

echo "$TAG Installing GiteaPC into $PREFIX"

# Download the source code.
WORKDIR="$(mktemp -d)"
trap 'rm -rf "$WORKDIR"' EXIT

cd "$WORKDIR"
git clone --depth=1 "$URL" giteapc
cd giteapc

python3 giteapc.py install Lephenixnoir/GiteaPC

echo "$TAG Installed GiteaPC in $PREFIX"
echo "$TAG Binary: $PREFIX/bin/giteapc"

echo "$TAG"
echo "$TAG To use this GiteaPC installation:"
echo "$TAG"
echo "$TAG   export HOME=\"$HOME\""
echo "$TAG   export PATH=\"$PREFIX/bin:\$PATH\""