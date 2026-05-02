#!/usr/bin/env bash
set -euo pipefail

source "$(dirname -- "$0")/config.sh"
: "${ssh_host:?} ${ssh_port:?} ${ssh_user:?} ${usb_devices[*]:?} ${usb_local_port:?} ${usb_remote_port:?}"

args=("$@")

echo "Environment: Linux"
echo "Devices: ${args[*]}"


if ! grep -q "^vhci_hcd" /proc/modules ; then
  sudo modprobe vhci_hcd
fi

requested_devices=()

for dev in "${args[@]}"; do
  if [[ ! -v usb_devices["$dev"] ]]; then
    echo "Unknown device '$dev'"
    exit 1
  fi

  requested_devices+=("${usb_devices["$dev"]}")
done

trap 'kill $!' EXIT
while IFS= read -r line; do
  echo "$line"

  if [[ $line == "FOUND-DEVICE:"* ]]; then
    sudo usbip --tcp-port "$usb_local_port" attach -r127.0.0.1 "-b${line#FOUND-DEVICE:}"
  fi
done < <(
# shellcheck disable=SC2087
ssh -L "127.0.0.1:$usb_local_port:127.0.0.1:$usb_remote_port" -p "$ssh_port" "$ssh_user@$ssh_host" bash -s -- "${requested_devices[@]@Q}" <<- 'EOF'
  for dev in "$@"; do
    dev=($dev)
    vendorId=${dev[0]}
    productId=${dev[1]}
    serial=${dev[@]:2}

    echo Searching for "${dev[*]}"

    find /sys/bus/usb/devices/* -maxdepth 1 -exec sh -c "{ grep -q \"$serial\$\" {}/serial && grep -q \"$vendorId\$\" {}/idVendor && grep -q \"$productId\$\" {}/idProduct ; } 2> /dev/null" \; -print | rev | cut -d/ -f1 | rev | sed 's/^/FOUND-DEVICE:/'
  done
  echo "Done! Press Ctl+C to stop the usb forwarding."
  sleep inf
EOF
)