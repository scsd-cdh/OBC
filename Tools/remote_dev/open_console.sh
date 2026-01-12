#!/usr/bin/env bash
set -euo pipefail

args=("$@")
target=${args[0]:-none}

source "$(dirname -- "$0")/config.sh"

echo "Environment: Linux"
echo "Target: $target"

cd "$(dirname -- "$0")/../.."

obc() {
  echo "====Console(START)===="
  # shellcheck disable=SC2087
  ssh -tt -p "$ssh_port" "$ssh_user@$ssh_host" <<- EOF
    cd remote_files
    TERM=xterm ulog-decoder -s auto CDH.elf
    exit
EOF
  echo "====Console(END)===="
  echo
}

case "$target" in
    obc)
        obc
        ;;
    *)
        echo
        echo "Unknown target, valid targets: obc"
        exit 1
        ;;
esac
