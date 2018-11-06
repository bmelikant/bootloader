#!/bin/bash
# builds boot image then runs bochs emulator with default floppy disk options
if [[ $UID != 0 ]]; then
    echo "This script requires administrator privileges to write to floppy disk drive:"
    echo "sudo $0 $*"
    exit 1
fi

rm -rf boot.bin
./image.sh
bochs 'floppya: 1_44=/dev/fd0,status=inserted'
