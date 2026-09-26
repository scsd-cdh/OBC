#!/usr/bin/env bash
set -euo pipefail

export MSYS_NO_PATHCONV=1

IMAGE_NAME="${IMAGE_NAME:-msp430-builder}"
DOCKERFILE="${DOCKERFILE:-dockerfile}"

MCU="${MCU:-msp430fr6989}" # IMPORTANT: Set it to correct MCU
EXTRA_MCU_FLAGS="${EXTRA_MCU_FLAGS--mlarge}"

echo "==> Building Docker image: $IMAGE_NAME"
docker build -t "$IMAGE_NAME" -f "$DOCKERFILE" .

echo "==> Building MSP430 firmware"
echo "    MCU=$MCU"
echo "    EXTRA_MCU_FLAGS=$EXTRA_MCU_FLAGS"

docker run --rm \
  -v "$PWD":/workspace \
  -w /workspace \
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
echo "  build/BMS.elf"
echo "  build/BMS.hex"
echo "  build/BMS.bin"