#!/usr/bin/env bash
set -euo pipefail

export MSYS_NO_PATHCONV=1

IMAGE_NAME="${IMAGE_NAME:-msp430-builder}"
DOCKERFILE="${DOCKERFILE:-dockerfile}"

MCU="${MCU:-msp430fr6989}"
EXTRA_MCU_FLAGS="${EXTRA_MCU_FLAGS--mlarge}"

# Directory containing this script == directory containing CMakeLists.txt
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"

# Mount this as /workspace. CMakeLists.txt walks up with ../.. to find
# DRIVERS/ and MIDDLEWARE/, so this must be the directory containing them.
MOUNT_DIR="$(cd -- "$SCRIPT_DIR/../.." && pwd)"

# Path of the CMakeLists.txt folder relative to the mount point.
SRC_REL="$(realpath --relative-to="$MOUNT_DIR" "$SCRIPT_DIR")"
CONTAINER_SRC="/workspace/$SRC_REL"

# Resolve Dockerfile path relative to this script.
if [[ "$DOCKERFILE" != /* ]]; then
  DOCKERFILE="$SCRIPT_DIR/$DOCKERFILE"
fi

# Convert a POSIX/MSYS path to the OS-native form Docker Desktop expects.
# No-op on Linux/macOS (no cygpath).
to_docker_path() {
  if command -v cygpath >/dev/null 2>&1; then
    cygpath -w "$1"
  else
    printf '%s' "$1"
  fi
}

HOST_MOUNT="$(to_docker_path "$MOUNT_DIR")"
HOST_DOCKERFILE="$(to_docker_path "$DOCKERFILE")"
HOST_CONTEXT="$(to_docker_path "$SCRIPT_DIR")"

echo "==> Build script dir:  $SCRIPT_DIR"
echo "==> Mounting:          $HOST_MOUNT  ->  /workspace"
echo "==> Container workdir: $CONTAINER_SRC"
echo "==> Dockerfile:        $HOST_DOCKERFILE"

echo "==> Building Docker image: $IMAGE_NAME"
docker build -t "$IMAGE_NAME" -f "$HOST_DOCKERFILE" "$HOST_CONTEXT"

echo "==> Building MSP430 firmware"
echo "    MCU=$MCU"
echo "    EXTRA_MCU_FLAGS=$EXTRA_MCU_FLAGS"

docker run --rm \
  -v "$HOST_MOUNT":/workspace \
  -w "$CONTAINER_SRC" \
  -e MCU="$MCU" \
  -e EXTRA_MCU_FLAGS="$EXTRA_MCU_FLAGS" \
  "$IMAGE_NAME" \
  bash -lc '
    set -e
    cmake -S . -B build \
      -DCMAKE_TOOLCHAIN_FILE=/opt/ti/msp430-toolchain.cmake \
      -DMCU="$MCU" \
      -DEXTRA_MCU_FLAGS="$EXTRA_MCU_FLAGS"
    cmake --build build -j"$(nproc)"
  '

echo "==> Done"
echo "Outputs:"
echo "  $SCRIPT_DIR/build/PDS.elf"
echo "  $SCRIPT_DIR/build/PDS.hex"
echo "  $SCRIPT_DIR/build/PDS.bin"