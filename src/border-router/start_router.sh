#!/bin/bash

if [ -n "$IOT_LAB_FRONTEND_FQDN" ]; then
  # RIOT doesnt support the arm gcc version installed by default on the SSH frontend, e.g. 4.9.
  # Therefore, we use arm gcc version 9 found in /opt/
  source /opt/riot.source
else
  echo "[ERROR] Not in IoT-LAB frontend"
fi

iotlab-experiment submit -n border_router -d 60 -l 1,archi=m3:at86rf231+site=grenoble
iotlab-experiment wait

# Build and flash the required firmware for the border router node
make ETHOS_BAUDRATE=500000 BOARD=iotlab-a8-m3 clean all -C ./gnrc_border_router
iotlab-ssh flash-m3 gnrc_border_router/bin/iotlab-a8-m3/gnrc_border_router.elf -l grenoble,m3,1

# Get the address of the border router node
target_address=$(iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes | tr -d '"[] ')
target=$(echo $target_address | cut -d'.' -f1)
target_node="node-$target"
echo "Target node:" $target_node

sudo ethos_uhcpd.py $target tap0 2001:660:3207:04c1::1/64
