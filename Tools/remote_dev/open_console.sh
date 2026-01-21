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
    cd remote_files
    FORCE_COLOR=1 TERM=xterm ulog-decoder -s /dev/ttyACM0 -b 115200 "${binary}.elf" 2>&1 | sed -u -r 's/WEST_TOPDIR\/[^\/]+\///' | ts '[%H:%M:%S]'
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
