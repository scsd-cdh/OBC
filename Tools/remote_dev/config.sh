# shellcheck disable=SC2034
ssh_host=iris.alexkar598.dev
ssh_port=43999
ssh_user=remote
tcl_port=9991
gdb_port=9992
usb_local_port=9993
usb_remote_port=3240

# VendorId ProductId Serial
declare -A usb_devices=(
    [msp1]="2047 0013 62979B6E0E000400"
)
