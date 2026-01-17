#!/usr/bin/env bash
set -euo pipefail

args=("$@")
target=${args[0]:-none}

source "$(dirname -- "$0")/config.sh"

echo "Environment: Linux"
echo "Target: $target"

cd "$(dirname -- "$0")/../.."

build_obc() {
  build_dir=${args[1]:-build}

  echo Build Dir: "$build_dir"
  echo

  echo "====Build(START)===="
  if ! command -v cmake >/dev/null 2>&1; then
    echo "Cannot find cmake, skipping build stage"
  else
    cmake --build "$build_dir"
  fi
  echo "====Build(END)===="
  echo

  echo "====Upload(START)===="
  elf_path="$build_dir/APP/CDH_App/CDH.elf"

  current_hash=$(ssh -p "$ssh_port" "$ssh_user@$ssh_host" -- sha1sum remote_files/CDH.elf | cut -f1 -d' ' || echo MISSING)
  new_hash=$(sha1sum "$elf_path" | cut -f1 -d' ')
  echo "Old file hash: $current_hash"
  echo "New file hash: $new_hash"
  if [ "$current_hash" != "$new_hash" ]; then
    sftp -b - -P "$ssh_port" "$ssh_user@$ssh_host" <<- EOF
      progress
      put "$elf_path" remote_files/CDH.elf
      put "ProjectFiles/CDH/openocd.cfg" remote_files/cdh_openocd.cfg
EOF
  else
    echo "File is already present, skipping upload"
  fi
  echo "====Upload(END)===="

  echo "====Flash(START)===="
  ssh -p "$ssh_port" "$ssh_user@$ssh_host" <<- EOF
    cd remote_files
    openocd -f cdh_openocd.cfg -c "telnet_port disabled" -c "tcl_port disabled" -c "gdb_port disabled" -c "program CDH.elf verify" -c reset -c shutdown
EOF
  echo "====Flash(END)===="
  echo

  echo "====Debug(START)===="
  # shellcheck disable=SC2087
  ssh -L 127.0.0.1:$gdb_port:127.0.0.1:$gdb_port -p "$ssh_port" "$ssh_user@$ssh_host" <<- EOF
    cd remote_files
    openocd -f cdh_openocd.cfg -c "telnet_port disabled" -c "tcl_port disabled" -c "gdb_port $gdb_port" -c init -c "reset run" -c "echo ====Debug(READY)===="
EOF
  echo "====Debug(END)===="
  echo
}

case "$target" in
    obc)
        build_obc
        ;;
    *)
        echo
        echo "Unknown target, valid targets: obc"
        exit 1
        ;;
esac
