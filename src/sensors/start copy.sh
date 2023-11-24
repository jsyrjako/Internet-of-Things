#!/bin/bash

BOARD=iotlab-m3
NODES=180-185
BR_ID=180
IPV6=2001:660:4403:0496
TAP=69
SENSORS=181-185
SITE=lille
PAN_ID=0xa48f
CHANNEL=24

if [ -n "$IOT_LAB_FRONTEND_FQDN" ]; then
  # RIOT doesnt support the arm gcc version installed by default on the SSH frontend, e.g. 4.9.
  # Therefore, we use arm gcc version 9 found in /opt/
  source /opt/riot.source
else
  echo "[ERROR] Not in IoT-LAB frontend"
fi

iotlab-experiment submit -n "sensors_ipv6" -d 120 -l ${SITE},m3,${NODES}
iotlab-experiment wait --timeout 30 --cancel-on-timeout

if [ -n "$IOT_LAB_FRONTEND_FQDN" ]; then
  # Make sure that the monitoring profile exists
  iotlab-profile del -n monitor_sensors_m3
  iotlab-profile addm3 -n monitor_sensors_m3 -voltage -current -power -period 8244 -avg 16
  iotlab-node --update-profile monitor_sensors_m3

  # Make the sensor nodes
  make -C ./measurer/ BOARD=$BOARD DEFAULT_CHANNEL=$CHANNEL DEFAULT_PAN_ID=$PAN_ID
  echo "Flashing Sensor Nodes"
  cp measurer/bin/${BOARD}/iot2023sensor.elf ~/shared/
  iotlab-node --flash ~/shared/iot2023sensor.elf -l ${SITE},m3,${SENSORS}

  make -C ./br/gnrc_border_router ETHOS_BAUDRATE=500000 BOARD=$BOARD DEFAULT_CHANNEL=$CHANNEL DEFAULT_PAN_ID=$PAN_ID IOTLAB_NODE=m3-${BR_ID}.${SITE}.iot-lab.info flash

  # Start ethos on the border router
  echo "Starting Ethos BR:${BR_ID}, TAP:${TAP}, IPV6:${IPV6}::/64"
  sudo ethos_uhcpd.py m3-${BR_ID} tap${TAP} ${IPV6}::/64

fi
