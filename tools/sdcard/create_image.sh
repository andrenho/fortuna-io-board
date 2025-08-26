#!/bin/sh

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 IMAGE_NAME SIZE_IN_MB"
  exit 1
fi

dd if=/dev/zero of=$1 bs=1M count=$2
mkfs.vfat $1