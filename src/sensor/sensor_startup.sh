#!/bin/bash

# Select the architecture you want to use, e.g. iotlab-m3, iotlab-a8-m3, iotlab-a8
ARCH=iotlab-m3

if [ -n "$IOT_LAB_FRONTEND_FQDN" ]; then
  # RIOT doesnt support the arm gcc version installed by default on the SSH frontend, e.g. 4.9.
  # Therefore, we use arm gcc version 9 found in /opt/
  source /opt/riot.source
else
  echo "[ERROR] Not in IoT-LAB frontend"
fi

make BOARD=${ARCH}

if [ -n "$IOT_LAB_FRONTEND_FQDN" ]; then
  cp bin/${ARCH}/iot2023joonajanne.elf ~/shared/

  iotlab-profile del -n monitor_sensors_m3
  iotlab-profile addm3 -n monitor_sensors_m3 -voltage -current -power -period 8244 -avg 16

  nodes=$(iotlab-experiment submit -n iot2023joonajanne -d 60 -l grenoble,m3,150-155,~/shared/iot2023joonajanne.elf,monitor_sensors_m3)
  iotlab-experiment wait --timeout 30 --cancel-on-timeout

  experiment_id=$(echo $nodes | jq '.id')

  iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes
  # iotlab-experiment stop -i $experiment_id
fi
