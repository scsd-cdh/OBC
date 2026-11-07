#!/usr/bin/env bash
set -euo pipefail

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
IMAGE_NAME="${IMAGE_NAME:-msp430-builder}"
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "$SCRIPT_DIR/../.." && pwd)"
FIRMWARE="$SCRIPT_DIR/build/PDS.hex"
MCU="${MCU:-msp430fr6989}"
PROBE_DEV="${PROBE_DEV:-}"
firmware_set=0
DEBUG_MODE=0            # 0 = flash, 1 = interactive mspdebug, 2 = gdb server
BUSID=""
USBIPD=""
GDB_SERVER_NAME=""
GDB_PORT=""
GDB_NETWORK=""
MSPDEBUG_TIMEOUT="${MSPDEBUG_TIMEOUT:-150}"

while [[ $# -gt 0 ]]; do
    case "$1" in
        --mcu)
            [[ $# -ge 2 ]] || {
                echo "error: --mcu requires a target" >&2
                exit 2
            }
            MCU="$2"
            shift 2
            ;;
        --help|-h)
            echo "usage: $0 [firmware] [--mcu msp430fr5989|msp430fr5969|msp430fr6989] [--debug|--gdb-server]"
            exit 0
            ;;
        --debug)
            DEBUG_MODE=1
            shift
            ;;
        --gdb-server)
            DEBUG_MODE=2
            shift
            ;;
        *)
            if [[ "$firmware_set" -eq 0 ]]; then
                FIRMWARE="$1"
                firmware_set=1
            else
                echo "error: unexpected argument '$1'" >&2
                exit 2
            fi
            shift
            ;;
    esac
done

if [[ "$FIRMWARE" != /* ]]; then
    FIRMWARE="$(cd -- "$(dirname -- "$FIRMWARE")" && pwd)/$(basename -- "$FIRMWARE")"
fi
FIRMWARE_CONTAINER="/workspace/$(realpath --relative-to="$REPO_ROOT" "$FIRMWARE")"

case "$MCU" in
    msp430fr5989|msp430fr5969|msp430fr6989) ;;
    *)
        echo "error: unsupported MCU '$MCU'" >&2
        echo "       supported: msp430fr5989, msp430fr5969, msp430fr6989" >&2
        exit 2
        ;;
esac

# Known TI MSP430 debug probe USB IDs, in preference order (first match wins):
#   MSP-FET       - standalone black JTAG/SBW debugger
#   MSP-FET430UIF - older USB FET
#   eZ-FET        - on-board debugger on LaunchPad boards
PROBE_CANDIDATES=(
    "2047:0203:MSP-FET"
    "2047:0014:MSP-FET"
    "2047:0010:MSP-FET430UIF"
    "2047:0013:eZ-FET"
)

# Backwards-compatible manual override: if the user sets PROBE_VID/PROBE_PID,
# use only that single probe.
if [[ -n "${PROBE_VID:-}" || -n "${PROBE_PID:-}" ]]; then
    PROBE_CANDIDATES=("${PROBE_VID:-2047}:${PROBE_PID:-0013}:custom")
fi

# ---------------------------------------------------------------------------
# Platform detection
# ---------------------------------------------------------------------------
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) PLATFORM="windows" ;;
    Linux*)                PLATFORM="linux" ;;
    *)
        echo "error: unsupported platform: $(uname -s)" >&2
        echo "       supported: Windows (Git Bash), Linux" >&2
        exit 1
        ;;
esac

echo "==> Platform: $PLATFORM"

# ---------------------------------------------------------------------------
# Cleanup (registered once at top level)
# ---------------------------------------------------------------------------
cleanup() {
    if [[ -n "${GDB_SERVER_NAME:-}" ]] && \
       docker ps -a --format '{{.Names}}' 2>/dev/null | grep -qx "$GDB_SERVER_NAME"; then
        echo "==> Stopping GDB server container ($GDB_SERVER_NAME)..."
        docker rm -f "$GDB_SERVER_NAME" >/dev/null 2>&1 || true
    fi
    if [[ -n "${BUSID:-}" ]] && [[ -n "${USBIPD:-}" ]]; then
        echo "==> Detaching probe (BUSID $BUSID) from WSL..."
        "$USBIPD" detach --busid "$BUSID" 2>/dev/null || true
    fi
}
trap cleanup EXIT
trap 'exit 130' INT TERM

# Git Bash on Windows rewrites leading-slash args to Windows paths.
# Only needed there.
if [[ "$PLATFORM" == "windows" ]]; then
    export MSYS_NO_PATHCONV=1

    # ---- Yellow hint: a WSL instance must be open before flashing --------
    if [[ -w /dev/tty ]]; then
        printf '\033[33m' >/dev/tty
        cat >/dev/tty <<'HINT'
###############################################################################
#                                                                             #
#   IMPORTANT: A WSL instance must be open and running before flashing!      #
#                                                                             #
#   usbipd-win attaches the MSP debug probe to a live WSL distribution.      #
#   If no WSL distro is running, the probe will be only partially            #
#   initialized and mspdebug will fail with "No unused FET found."           #
#                                                                             #
#   Open a WSL terminal now (e.g. `wsl -d Debian`) and leave it running,     #
#   then re-run this script.                                                 #
#                                                                             #
###############################################################################
HINT
        printf '\033[0m\n' >/dev/tty
    else
        printf '\033[33m' >&2
        echo "IMPORTANT: a WSL instance must be open and running before flashing!" >&2
        printf '\033[0m\n' >&2
    fi
fi

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------
ps() {
    powershell.exe -NoProfile -Command "$1"
}

_detect_linux() {
    local entry vid pid name dev
    for entry in "${PROBE_CANDIDATES[@]}"; do
        IFS=: read -r vid pid name <<< "$entry"
        for dev in /sys/bus/usb/devices/*; do
            [[ -f "$dev/idVendor" ]]  || continue
            [[ -f "$dev/idProduct" ]] || continue
            if [[ "$(cat "$dev/idVendor")"  == "$vid" ]] && \
               [[ "$(cat "$dev/idProduct")" == "$pid" ]]; then
                printf '%s %s %s /dev/bus/usb/%03d/%03d\n' \
                    "$vid" "$pid" "$name" \
                    "$(cat "$dev/busnum")" "$(cat "$dev/devnum")"
                return 0
            fi
        done
    done
    return 1
}

_detect_windows() {
    local entry vid pid name out
    for entry in "${PROBE_CANDIDATES[@]}"; do
        IFS=: read -r vid pid name <<< "$entry"
        out=$(docker run --rm --privileged -v /sys:/sys:ro alpine sh -c "
          for dev in /sys/bus/usb/devices/*; do
            [ -f \"\$dev/idVendor\" ]  || continue
            [ -f \"\$dev/idProduct\" ] || continue
            if [ \"\$(cat \$dev/idVendor)\"  = \"$vid\" ] && \
               [ \"\$(cat \$dev/idProduct)\" = \"$pid\" ]; then
              printf '/dev/bus/usb/%03d/%03d\n' \"\$(cat \$dev/busnum)\" \"\$(cat \$dev/devnum)\"
              break
            fi
          done
        " 2>/dev/null) || true
        if [[ -n "$out" ]]; then
            printf '%s %s %s %s\n' "$vid" "$pid" "$name" "$out"
            return 0
        fi
    done
    return 1
}

detect_probe() {
    if [[ "$PLATFORM" == "linux" ]]; then
        _detect_linux
    else
        _detect_windows
    fi
}

# ===========================================================================
# Windows path: usbipd-win is required to bridge the USB probe into WSL2.
# ===========================================================================

if [[ "$PLATFORM" == "windows" ]]; then
    if command -v usbipd >/dev/null 2>&1; then
        USBIPD="usbipd"
    elif [[ -x "/c/Program Files/usbipd-win/usbipd.exe" ]]; then
        USBIPD="/c/Program Files/usbipd-win/usbipd.exe"
    else
        echo "==> usbipd-win is not installed."
        echo "    Installing via winget (this may prompt for administrator approval)..."
        ps "winget install --interactive --exact dorssel.usbipd-win"

        if [[ -x "/c/Program Files/usbipd-win/usbipd.exe" ]]; then
            USBIPD="/c/Program Files/usbipd-win/usbipd.exe"
        elif command -v usbipd >/dev/null 2>&1; then
            USBIPD="usbipd"
        else
            echo "    Could not locate usbipd after install. Reopen the terminal and retry."
            exit 1
        fi
    fi

    echo "==> usbipd-win located: $USBIPD"

    BOUND=""
    while IFS= read -r line; do
        if [[ "$line" =~ ^([0-9]+-[0-9]+)[[:space:]]+([0-9a-fA-F]{4}:[0-9a-fA-F]{4})[[:space:]] ]]; then
            busid="${BASH_REMATCH[1]}"
            vidpid="${BASH_REMATCH[2]}"
            if echo "$line" | grep -qiE 'Not shared'; then
                continue
            fi
            for entry in "${PROBE_CANDIDATES[@]}"; do
                IFS=: read -r vid pid name <<< "$entry"
                if [[ "${vidpid,,}" == "${vid,,}:${pid,,}" ]]; then
                    BUSID="$busid"
                    BOUND="$line"
                    break 2
                fi
            done
        fi
    done < <("$USBIPD" list 2>/dev/null)

    if [[ -z "$BUSID" ]]; then
        echo "==> No shared MSP/FET probe found."
        echo ""
        echo "    Current usbipd state:"
        "$USBIPD" list || true
        echo ""
        echo "    If your probe shows as 'Not shared' under 'Connected:',"
        echo "    bind it once in an ADMIN PowerShell:"
        echo "      usbipd bind --busid <BUSID> --force"
        exit 1
    fi

    echo "==> Probe is bound: $BOUND"
    echo "==> Using BUSID: $BUSID"

    echo "==> Attaching probe (BUSID $BUSID) to WSL..."
    if ! "$USBIPD" attach --wsl --busid "$BUSID"; then
        echo "error: usbipd attach failed." >&2
        exit 1
    fi
    sleep 3
fi

# ===========================================================================
# Common path: detect device, then flash / debug / serve
# ===========================================================================

PROBE_NAME=""
if [[ -z "$PROBE_DEV" ]]; then
    echo "==> Detecting MSP debug probe..."
    for entry in "${PROBE_CANDIDATES[@]}"; do
        IFS=: read -r _v _p _n <<< "$entry"
        echo "      looking for: $_n ($_v:$_p)"
    done

    detected="$(detect_probe || true)"

    if [[ -n "$detected" ]]; then
        read -r PROBE_VID PROBE_PID PROBE_NAME PROBE_DEV <<< "$detected"
        echo "    Detected $PROBE_NAME ($PROBE_VID:$PROBE_PID) at $PROBE_DEV"
    else
        echo "error: could not auto-detect any MSP debug probe." >&2
        if [[ "$PLATFORM" == "linux" ]]; then
            echo "  - Is the LaunchPad or MSP-FET plugged in?" >&2
            echo "  - Is the cdc_acm driver claiming it? Try:" >&2
            echo "      lsusb | grep -i 2047" >&2
        else
            echo "  - Is the probe attached via usbipd?" >&2
            echo "      usbipd list" >&2
        fi
        echo "  - Or set PROBE_DEV manually, e.g.:" >&2
        echo "      PROBE_DEV=/dev/bus/usb/001/004 ./flash.sh" >&2
        exit 1
    fi
else
    echo "==> Using PROBE_DEV from environment: $PROBE_DEV"
fi

if ! docker run --rm --device="$PROBE_DEV" alpine test -e "$PROBE_DEV" 2>/dev/null; then
    echo "error: $PROBE_DEV is not accessible in a container." >&2
    exit 1
fi

if [[ "$DEBUG_MODE" -eq 0 ]]; then
    if [[ ! -f "$FIRMWARE" ]]; then
        echo "error: firmware not found: $FIRMWARE" >&2
        echo "hint: run ./build.sh first" >&2
        exit 1
    fi
fi

# ---- Flash / debug / gdb-server -----------------------------------------
if [[ "$DEBUG_MODE" -eq 2 ]]; then
    GDB_SERVER_NAME="${GDB_SERVER_NAME:-msp430-gdb-server}"
    GDB_PORT="${GDB_PORT:-2000}"
    GDB_NETWORK="${GDB_NETWORK:-msp430-debug}"

    echo "==> Starting mspdebug GDB server (container: $GDB_SERVER_NAME, network: $GDB_NETWORK, port $GDB_PORT)"
    if [[ -n "$PROBE_NAME" ]]; then
        echo "    probe:        $PROBE_NAME"
    fi
    echo "    probe device: $PROBE_DEV"

    # Create a user-defined bridge network so another container can reach
    # this server by its container name (Docker embedded DNS).
    docker network create "$GDB_NETWORK" >/dev/null 2>&1 || true
    docker rm -f "$GDB_SERVER_NAME" >/dev/null 2>&1 || true

    docker run -dit \
        --name "$GDB_SERVER_NAME" \
        --network "$GDB_NETWORK" \
        --privileged \
        --device="$PROBE_DEV" \
        -v "$REPO_ROOT":/workspace \
        -w /workspace \
        "$IMAGE_NAME" \
        timeout --signal=TERM --kill-after=10s "${MSPDEBUG_TIMEOUT}s" \
            mspdebug tilib "gdb $GDB_PORT"

    echo "==> Waiting for the GDB server to exit (kill me or the container to stop)..."
    # Blocks until the container is removed/killed. Preserve diagnostics before
    # the EXIT trap removes the container.
    docker wait "$GDB_SERVER_NAME" >/dev/null 2>&1 || true
    server_status="$(docker inspect --format '{{.State.ExitCode}}' "$GDB_SERVER_NAME" 2>/dev/null || echo 1)"
    server_status="${server_status:-1}"
    if [[ "$server_status" != "0" ]]; then
        echo "error: mspdebug GDB server exited with status $server_status." >&2
        docker logs --tail 50 "$GDB_SERVER_NAME" >&2 || true
        exit "$server_status"
    fi
elif [[ "$DEBUG_MODE" -eq 1 ]]; then
    echo "==> Opening interactive mspdebug shell"
    if [[ -n "$PROBE_NAME" ]]; then
        echo "    probe:        $PROBE_NAME"
    fi
    echo "    probe device: $PROBE_DEV"
    echo "    type 'help' inside for commands, 'exit' or Ctrl+D to quit"
    echo ""

    docker run --rm -it \
        --privileged \
        --device="$PROBE_DEV" \
        -v "$REPO_ROOT":/workspace \
        -w /workspace \
        "$IMAGE_NAME" \
        mspdebug tilib
else
    echo "==> Flashing $FIRMWARE to $MCU"
    if [[ -n "$PROBE_NAME" ]]; then
        echo "    probe:        $PROBE_NAME"
    fi
    echo "    probe device: $PROBE_DEV"

    docker run --rm \
        --privileged \
        --device="$PROBE_DEV" \
        -v "$REPO_ROOT":/workspace \
        -w /workspace \
        "$IMAGE_NAME" \
        mspdebug tilib "prog $FIRMWARE_CONTAINER"
fi

echo "==> Done"