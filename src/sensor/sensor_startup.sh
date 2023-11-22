#!/bin/bash

# Select the architecture you want to use, e.g. iotlab-m3, iotlab-a8-m3, iotlab-a8
ARCH=iotlab-m3

if [ -n "$IOT_LAB_FRONTEND_FQDN" ]; then
  # RIOT doesn’t support the arm gcc version installed by default on the SSH frontend, e.g. 4.9.
  # Therefore, we use arm gcc version 9 found in /opt/
  source /opt/riot.source
else
  echo "[ERROR] Not in IoT-LAB frontend"
fi

make BOARD=${ARCH}


