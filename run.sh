#!/bin/bash

RAM_SIZE=5120
NVM_SIZE=2048
MAX_SIZE=$(($RAM_SIZE+$NVM_SIZE))

qemu-system-x86_64 \
    -hda image.qcow2 \
    -machine accel=kvm,nvdimm \
    -cpu host -smp 10 \
    -m $RAM_SIZE,slots=3,maxmem=${MAX_SIZE}m \
    -object memory-backend-file,id=mem1,mem-path=nvdimm_backend,size=${NVM_SIZE}m,pmem=on \
    -device nvdimm,id=nvdimm1,memdev=mem1 \
    -net user,hostfwd=tcp:127.0.0.1:5000-:22 \
    -net nic \
    -boot d \
    --daemonize
