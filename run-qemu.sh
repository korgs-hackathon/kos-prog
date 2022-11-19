#!/bin/bash


/opt/KasperskyOS-Community-Edition-1.1.0.356/toolchain/bin/qemu-system-aarch64 -m 2048 \
-machine vexpress-a15,secure=on -cpu cortex-a72 -nographic -monitor none -smp 4 -nic user \
-serial stdio -sd /workspace/program/build/einit/sdcard0.img \
-netdev tap,id=net0,ifname=tap0,script=no,downscript=no -net nic,model=lan9118,netdev=net0 \
-kernel /workspace/program/build/einit/kos-qemu-image