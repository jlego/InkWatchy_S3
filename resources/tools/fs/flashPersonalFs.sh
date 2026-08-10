#!/bin/bash

size=$(<in/size.txt tr -d '\n')
offset=$(<in/offset.txt tr -d '\n')

source ../globalFunctions.sh
serial_port=$(extract_serial_port "../other/in/esptool")
if [[ $? -ne 0 || -z "$serial_port" ]]; then
    echo "Failed to detect a valid serial port" >&2
    exit 1
fi

esptool_cmd=$(find_esptool)
if [[ -z "$esptool_cmd" ]]; then
    esptool_cmd="../other/in/esptool"
fi

./in/mklittlefs -c out/fsDump -s $size out/fsPers.bin
$esptool_cmd --port $serial_port write_flash $offset out/fsPers.bin