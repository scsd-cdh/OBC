#!/usr/bin/env bash
set -euo pipefail

args=("$@")
target=${args[0]:-none}

source "$(dirname -- "$0")/config.sh"
: "${ssh_host:?} ${ssh_port:?} ${ssh_user:?}"

echo "Environment: Linux"
echo "Target: $target"

cd "$(dirname -- "$0")/../.."

samv71() {
  binary=$1

  echo "====Console(START)===="
  # shellcheck disable=SC2087
  ssh -tt -p "$ssh_port" "$ssh_user@$ssh_host" <<- EOF
    echo "remote_files/${binary}_dictionary.json" | FORCE_COLOR=1 TERM=xterm entr -r -n python3 ~/zephyr_log_parser/live_log_parser.py --debug "remote_files/${binary}_dictionary.json" serial /dev/serial/by-id/usb-Atmel_Corp._EDBG_CMSIS-DAP_* 115200
    exit
EOF
  echo "====Console(END)===="
  echo
}

case "$target" in
    cdh)
        samv71 CDH
        ;;
    comms)
        samv71 COMMS
        ;;
    *)
        echo
        echo "Unknown target, valid targets: cdh, comms"
        exit 1
        ;;
esac
