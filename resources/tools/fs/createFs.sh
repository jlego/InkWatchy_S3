#!/bin/bash -x
source ../globalFunctions.sh

size=$(<in/size.txt tr -d '\n')
offset=$(<in/offset.txt tr -d '\n')

if ! check_define FILESYSTEM_COMPRESSION ../../../src/defines/config.h; then
    echo "FILESYSTEM_COMPRESSION is enabled. Applying compression..."
    gcc -o in/compressFile -I../../../components/tamp/ ../../../components/tamp/tamp/common.c ../../../components/tamp/tamp/compressor.c compressFile.c
    if [ $? -ne 0 ]; then
        echo "Error compiling compressFile.c. Exiting."
        exit 1
    fi
    rm -rf /tmp/littlefs
    cp -r littlefs /tmp/
    find /tmp/littlefs -type f ! -path "*/book/*" ! -name ".keep" -exec bash -c './in/compressFile "$0" "$0.compressed" && mv "$0.compressed" "$0"' {} \;
    rm -rf out/fs.bin
    ./in/mklittlefs --all-files -c /tmp/littlefs -s $size out/fs.bin
    rm -rf /tmp/littlefs
else
    echo "FILESYSTEM_COMPRESSION is not enabled. Skipping compression."
    rm -rf out/fs.bin
    ./in/mklittlefs --all-files -c littlefs -s $size out/fs.bin
fi