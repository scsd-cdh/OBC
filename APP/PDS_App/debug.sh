#!/usr/bin/env bash
set -euo pipefail

# Git Bash on Windows rewrites leading-slash args (e.g. /workspace) to
# Windows paths. Disable that so docker sees them verbatim.
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) export MSYS_NO_PATHCONV=1 ;;
esac

# ---------------------------------------------------------------------------
# debug.sh — flash (optional) + launch msp430-elf-gdb against a mspdebug
#            GDB server that is started automatically via ./flash.sh.
# ---------------------------------------------------------------------------

IMAGE_NAME="${IMAGE_NAME:-msp430-builder}"
MCU="${MCU:-msp430fr6989}"
DEFAULT_ELF="build/PDS.elf"          # default produced by ./build.sh
FIRMWARE=""
ELF=""
firmware_set=0
elf_set=0
mcu_set=0

while [[ $# -gt 0 ]]; do
    case "$1" in
        --mcu)
            [[ $# -ge 2 ]] || { echo "error: --mcu requires a target" >&2; exit 2; }
            MCU="$2"; mcu_set=1; shift 2 ;;
        --elf)
            [[ $# -ge 2 ]] || { echo "error: --elf requires a path" >&2; exit 2; }
            ELF="$2"; elf_set=1; shift 2 ;;
        --help|-h)
            cat <<'EOF'
usage: ./debug.sh [firmware.hex] [--elf path/to.elf] [--mcu msp430fr6989]

Flashes firmware (optional), then starts the mspdebug GDB server via
./flash.sh --gdb-server and connects msp430-elf-gdb to it.

Symbol resolution order:
  1. --elf <path>              explicit override
  2. <firmware>.elf            auto-derived from an explicit .hex argument
  3. build/PDS.elf             default produced by ./build.sh

Before handing control to you, GDB (when an ELF is present) programs the
device from that ELF and resets it, so the chip always matches the symbols
GDB is using. Without an ELF, GDB just resets the device.

  firmware.hex   If given, flashed via ./flash.sh first.
  --elf <path>   ELF file with symbols.
  --mcu  <mcu>   Target MCU (msp430fr5989|msp430fr5969|msp430fr6989).

Inside GDB:
  continue (c)   run the target
  break <fn>     breakpoint on a symbol
  info regs      registers
  monitor reset  reset the target
  quit           disconnect (the server is torn down automatically)
EOF
            exit 0 ;;
        *)
            if [[ "$firmware_set" -eq 0 ]]; then
                FIRMWARE="$1"; firmware_set=1
            else
                echo "error: unexpected argument '$1'" >&2; exit 2
            fi
            shift ;;
    esac
done

# ELF resolution (only when --elf wasn't given):
#   explicit .hex  ->  <firmware>.elf if it exists
#   otherwise      ->  build/PDS.elf if it exists (default build output)
if [[ "$elf_set" -eq 0 ]]; then
    if [[ -n "$FIRMWARE" ]]; then
        candidate="${FIRMWARE%.hex}.elf"
        if [[ -f "$candidate" ]]; then
            ELF="$candidate"
        fi
    fi
    if [[ -z "$ELF" && -f "$DEFAULT_ELF" ]]; then
        ELF="$DEFAULT_ELF"
    fi
fi

FORWARD_ARGS=()
[[ "$mcu_set" -eq 1 ]] && FORWARD_ARGS+=(--mcu "$MCU")

# Server naming must match flash.sh defaults.
GDB_SERVER_NAME="${GDB_SERVER_NAME:-msp430-gdb-server}"
GDB_NETWORK="${GDB_NETWORK:-msp430-debug}"
GDB_PORT="${GDB_PORT:-2000}"

# Ensure the network exists even if flash.sh hasn't created it yet.
docker network create "$GDB_NETWORK" >/dev/null 2>&1 || true

# ---------------------------------------------------------------------------
# Optional pre-flash (single attach cycle of its own)
# ---------------------------------------------------------------------------
if [[ -n "$FIRMWARE" ]]; then
    echo "==> Flashing $FIRMWARE before starting the debug session..."
    ./flash.sh "$FIRMWARE" "${FORWARD_ARGS[@]}"
    echo ""
fi

# ---------------------------------------------------------------------------
# Start GDB server via flash.sh in the background
# ---------------------------------------------------------------------------
echo "==> Starting mspdebug GDB server via ./flash.sh --gdb-server..."
./flash.sh --gdb-server "${FORWARD_ARGS[@]}" &
FLASH_PID=$!

# On exit/INT/TERM, tear everything down:
#   - removing the container unblocks flash.sh's `docker wait`
#   - flash.sh's own EXIT trap detaches the usbipd probe
SERVER_CLEANUP() {
    if docker ps -a --format '{{.Names}}' 2>/dev/null | grep -qx "$GDB_SERVER_NAME"; then
        docker rm -f "$GDB_SERVER_NAME" >/dev/null 2>&1 || true
    fi
    if kill -0 "$FLASH_PID" 2>/dev/null; then
        wait "$FLASH_PID" 2>/dev/null || true
    fi
}
trap SERVER_CLEANUP EXIT INT TERM

# ---------------------------------------------------------------------------
# Wait for mspdebug to say it's listening.
#
# flash.sh creates the container asynchronously, so on early iterations the
# container may not exist yet; only "was running, then exited" is fatal.
# ---------------------------------------------------------------------------
echo "==> Waiting for the GDB server to bind port $GDB_PORT..."
BOUND=0
SEEN=0
for _ in $(seq 1 120); do
    if docker ps -a --format '{{.Names}}' 2>/dev/null | grep -qx "$GDB_SERVER_NAME"; then
        SEEN=1
        if docker ps --format '{{.Names}}' 2>/dev/null | grep -qx "$GDB_SERVER_NAME"; then
            if docker logs --tail 50 "$GDB_SERVER_NAME" 2>&1 | grep -q "Bound to port"; then
                BOUND=1
                break
            fi
        else
            break
        fi
    fi
    sleep 0.5
done

if [[ "$BOUND" -ne 1 ]]; then
    echo "error: GDB server did not bind port $GDB_PORT." >&2
    if [[ "$SEEN" -eq 1 ]]; then
        echo "---- container log (last 30 lines) ----" >&2
        docker logs --tail 30 "$GDB_SERVER_NAME" 2>&1 >&2 || true
        echo "---------------------------------------" >&2
    else
        echo "error: server container was never created; check flash.sh output above." >&2
    fi
    exit 1
fi
echo "    server is up."

# ---------------------------------------------------------------------------
# Launch msp430-elf-gdb on the same user-defined network.
#
# The ELF is passed as a positional argument (not via -ex "file ..."):
# positional files are loaded before any -ex commands run, so the symbol
# table is in place before we connect to the remote — and there is no
# duplicate "file" command to trigger GDB's
#   "A program is being debugged already. Are you sure you want to change
#    the file?" prompt.
#
# After connecting, we flash the exact ELF GDB has symbols for (load) and
# reset the target (monitor reset), so the chip is guaranteed to match the
# symbols and start cleanly from the reset vector before the user takes
# control. load requires an ELF; monitor reset works either way.
# ---------------------------------------------------------------------------
GDB_ARGS=()
if [[ -n "$ELF" ]]; then
    if [[ "$elf_set" -eq 0 && "$ELF" == "$DEFAULT_ELF" ]]; then
        echo "==> Loading default symbols: $ELF"
    else
        echo "==> Loading symbols: $ELF"
    fi
    GDB_ARGS+=("$ELF")
else
    echo "    note: no .elf found (looked for ${DEFAULT_ELF}); GDB will start without symbols."
fi
GDB_ARGS+=(-ex "target remote $GDB_SERVER_NAME:$GDB_PORT")

if [[ -n "$ELF" ]]; then
    echo "==> GDB will flash the device from $ELF and reset it before giving you control"
    GDB_ARGS+=(-ex "load")
else
    echo "==> GDB will reset the device before giving you control"
fi
GDB_ARGS+=(-ex "monitor reset")

echo "==> Launching msp430-elf-gdb"
echo "    (inside GDB: 'continue' to run, 'quit' to exit)"
echo ""
docker run --rm -it \
    --network "$GDB_NETWORK" \
    -v "$PWD":/workspace \
    -w /workspace \
    "$IMAGE_NAME" \
    msp430-elf-gdb "${GDB_ARGS[@]}"

echo ""
echo "==> GDB session ended."