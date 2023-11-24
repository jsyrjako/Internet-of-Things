#!/bin/bash

BOARD=iotlab-m3
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

iotlab-experiment submit -n "sensors_ipv6" -d 120 -l m3,$SITE,180-185
iotlab-experiment wait --timeout 30 --cancel-on-timeout

# Make the sensor nodes
make -C ./measurer/ BOARD=$BOARD DEFAULT_CHANNEL=$CHANNEL DEFAULT_PAN_ID=$PAN_ID
make -C ./br/gnrc_border_router ETHOS_BAUDRATE=500000 BOARD=$BOARD DEFAULT_CHANNEL=$CHANNEL DEFAULT_PAN_ID=$PAN_ID IOTLAB_NODE=m3-${BR_ID}.${SITE}.iot-lab.info flash

if [ -n "$IOT_LAB_FRONTEND_FQDN" ]; then
  # Make sure that the monitoring profile exists
  iotlab-profile del -n monitor_sensors_m3
  iotlab-profile addm3 -n monitor_sensors_m3 -voltage -current -power -period 8244 -avg 16
  iotlab-node --update-profile monitor_sensors_m3

  # Flash the sensor nodes
  echo "Flashing Sensor Nodes"
  cp measurer/bin/${BOARD}/iot2023sensor.elf ~/shared/
  iotlab-node --flash ~/shared/iot2023sensor.elf -l ${SITE},m3,${SENSORS}

  # Start ethos on the border router
  sudo ethos_uhcpd.py m3-${BR_ID} tap${TAP} ${IPV6}::/64

fi
