#!/bin/bash
if [[ $UID != 0 ]]; then
    echo "Please run this script with sudo:"
    echo "sudo $0 $*"
    exit 1
fi

nasm boot12.asm -o boot.bin -f bin
dd if=boot.bin of=/dev/fd0
