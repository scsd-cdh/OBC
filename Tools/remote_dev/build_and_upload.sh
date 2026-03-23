#!/usr/bin/env bash
set -euo pipefail

args=("$@")
target=${args[0]:-none}

source "$(dirname -- "$0")/config.sh"
: "${ssh_host:?} ${ssh_port:?} ${ssh_user:?} ${tcl_port:?} ${gdb_port:?}"


echo "Environment: Linux"
echo "Target: $target"

cd "$(dirname -- "$0")/../.."

if [[ -d ../venv ]]; then
  source ../venv/bin/activate
elif [[ -d ../.venv ]]; then
  source ../.venv/bin/activate
fi

build_zephyr() {
  binary=$1

  build_dir=${args[1]:-build}

  echo Build Dir: "$build_dir"
  echo

  echo "====Build(START)===="
  if ! command -v west >/dev/null 2>&1; then
    echo "Cannot find west, skipping build stage"
  else
    pushd "APP/${binary}_App"
    west build -d "$build_dir" -- -GNinja -DZEPHYR_SCA_VARIANT=dtdoctor
    popd
  fi
  echo "====Build(END)===="
  echo

  echo "====Upload(START)===="
  elf_path="APP/${binary}_App/$build_dir/zephyr/zephyr.elf"
  logdb_path="APP/${binary}_App/$build_dir/zephyr/log_dictionary.json"

  current_hash=$(ssh -p "$ssh_port" "$ssh_user@$ssh_host" -- sha1sum "remote_files/${binary}.elf" | cut -f1 -d' ' || echo MISSING)
  new_hash=$(sha1sum "$elf_path" | cut -f1 -d' ')
  echo "Old file hash: $current_hash"
  echo "New file hash: $new_hash"
  if [ "$current_hash" != "$new_hash" ]; then
    sftp -C -b - -P "$ssh_port" "$ssh_user@$ssh_host" <<- EOF
      progress
      put "$elf_path" remote_files/${binary}.elf
      put "$logdb_path" remote_files/${binary}_dictionary.json
EOF
  else
    echo "File is already present, skipping upload"
  fi
  echo "====Upload(END)===="

  echo "====Flash(START)===="
  # shellcheck disable=SC2087
  ssh -p "$ssh_port" "$ssh_user@$ssh_host" <<- EOF
    printf '%s\x1a' 'capture "program ${binary}.elf preverify verify reset"' | nc -q 1 127.0.0.1 $tcl_port | tr '\32' '\n'
    echo "Done!"
EOF
  echo "====Flash(END)===="
  echo

  echo "====Debug(START)===="
  # shellcheck disable=SC2087
  echo "Establishing background gdb tunnel (no further messages will be displayed)"
  ssh -N -L "127.0.0.1:$gdb_port:127.0.0.1:$gdb_port" -p "$ssh_port" "$ssh_user@$ssh_host"
  echo "====Debug(END)===="
  echo
}

case "$target" in
    cdh)
        build_zephyr CDH
        ;;
    comms)
        build_zephyr COMMS
        ;;
    *)
        echo
        echo "Unknown target, valid targets: cdh, comms"
        exit 1
        ;;
esac
