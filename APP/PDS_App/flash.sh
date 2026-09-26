#!/usr/bin/env bash
set -euo pipefail

# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------
IMAGE_NAME="${IMAGE_NAME:-msp430-builder}"
FIRMWARE="${1:-build/BMS.hex}"
MCU="${MCU:-msp430fr6989}"
PROBE_DEV="${PROBE_DEV:-}"

# Known TI MSP430 debug probe USB IDs, in preference order (first match wins):
#   MSP-FET       - standalone black JTAG/SBW debugger
#   MSP-FET430UIF - older USB FET
#   eZ-FET        - on-board debugger on LaunchPad boards
PROBE_CANDIDATES=(
    "2047:0203:MSP-FET"
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

# Git Bash on Windows rewrites leading-slash args to Windows paths.
# Only needed there.
if [[ "$PLATFORM" == "windows" ]]; then
    export MSYS_NO_PATHCONV=1
fi

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------
ps() {
    powershell.exe -NoProfile -Command "$1"
}

# ---- Linux: scan /sys directly -------------------------------------------
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

# ---- Windows (Docker Desktop VM): scan /sys via a helper container --------
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

# Detect a TI MSP430 debug probe. On success, prints:
#   <vid> <pid> <name> <device-path>
detect_probe() {
    if [[ "$PLATFORM" == "linux" ]]; then
        _detect_linux
    else
        _detect_windows
    fi
}

# ===========================================================================
# Windows path: usbipd-win is required to bridge the USB probe into WSL2,
# because Docker Desktop on Windows has no native USB passthrough.
# ===========================================================================
BUSID=""

if [[ "$PLATFORM" == "windows" ]]; then
    # ---- Stage 1: Locate usbipd-win ---------------------------------------
    USBIPD=""
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

    # ---- Stage 2: Find the currently-connected, shared MSP/FET probe ------
    BOUND=""
    while IFS= read -r line; do
        if [[ "$line" =~ ^([0-9]+-[0-9]+)[[:space:]] ]]; then
            if echo "$line" | grep -qiE 'MSP|FET' && \
               ! echo "$line" | grep -qiE 'Not shared'; then
                BUSID="${BASH_REMATCH[1]}"
                BOUND="$line"
                break
            fi
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

    # ---- Cleanup: detach the probe on any exit ----------------------------
    cleanup() {
        if [[ -n "${BUSID:-}" ]]; then
            echo "==> Detaching probe (BUSID $BUSID) from WSL..."
            "$USBIPD" detach --busid "$BUSID" 2>/dev/null || true
        fi
    }
    trap cleanup EXIT

    # ---- Stage 3: Attach probe (single-shot) ------------------------------
    echo "==> Attaching probe (BUSID $BUSID) to WSL..."
    if ! "$USBIPD" attach --wsl --busid "$BUSID"; then
        echo "error: usbipd attach failed." >&2
        exit 1
    fi
    sleep 3
fi

# ===========================================================================
# Common path (both platforms): detect device, flash
# ===========================================================================

# ---- Detect the raw USB device path --------------------------------------
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

# ---- Sanity check: is the device visible in a plain container? -----------
if ! docker run --rm --device="$PROBE_DEV" alpine test -e "$PROBE_DEV" 2>/dev/null; then
    echo "error: $PROBE_DEV is not accessible in a container." >&2
    exit 1
fi

# ---- Sanity check: firmware exists ---------------------------------------
if [[ ! -f "$FIRMWARE" ]]; then
    echo "error: firmware not found: $FIRMWARE" >&2
    echo "hint: run ./build.sh first" >&2
    exit 1
fi

# ---- Flash ---------------------------------------------------------------
echo "==> Flashing $FIRMWARE to $MCU"
if [[ -n "$PROBE_NAME" ]]; then
    echo "    probe:        $PROBE_NAME"
fi
echo "    probe device: $PROBE_DEV"

docker run --rm \
    --privileged \
    --device="$PROBE_DEV" \
    -v "$PWD":/workspace \
    -w /workspace \
    "$IMAGE_NAME" \
    MSP430Flasher -n "$MCU" -w "$FIRMWARE" -v -z '[VCC,RESET]'

echo "==> Done"