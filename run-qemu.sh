#!/bin/bash


/opt/KasperskyOS-Community-Edition-1.1.0.356/toolchain/bin/qemu-system-aarch64 -m 2048 \
-machine vexpress-a15,secure=on -cpu cortex-a72 -nographic -monitor none -smp 4 \
-serial stdio \
-kernel ~/kos_workspace/program/build/einit/kos-qemu-image