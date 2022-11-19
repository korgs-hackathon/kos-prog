#!/bin/bash

echo "Mounting disk"
sudo mount /dev/sdb1 ~/mnt/fat32/
echo "Copying image"
sudo cp build/einit/kos-image ~/mnt/fat32/kos-image
sync
echo "Unmounting"
sudo umount ~/mnt/fat32