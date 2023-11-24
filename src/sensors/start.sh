#!/bin/bash

BOARD=iotlab-m3                 # Board architecture        iotlab-m3|iotlab-a8-m3|iotlab-a8
NODES=180-185                   # Sensor node IDs           site specific
BR_ID=180                       # Border router node ID     site specific
IPV6=2001:660:4403:0496         # IPv6 prefix               site specific
TAP=69                          # TAP interface number      0-255
SENSORS=181-185                 # Sensor node IDs           1-10
SITE=lille                      # IoT-LAB site              lille|grenoble|saclay|strasbourg
PAN_ID=0xa48f                   # 802.15.4 PAN ID           0x0000-0xffff
CHANNEL=24                      # 802.15.4 channel          11-26
ARCH=m3                         # Board architecture

if [ -n "$IOT_LAB_FRONTEND_FQDN" ]; then
  # RIOT doesnt support the arm gcc version installed by default on the SSH frontend, e.g. 4.9.
  # Therefore, we use arm gcc version 9 found in /opt/
  source /opt/riot.source
else
  echo "[ERROR] Not in IoT-LAB frontend"
fi

# iotlab-experiment submit -n "sensors_ipv6" -d 120 -l archi=m3:at86rf231+site=$SITE
# iotlab-experiment wait --timeout 30 --cancel-on-timeout

# Make the sensor nodes
make -C ./measurer/ BOARD=$BOARD DEFAULT_CHANNEL=$CHANNEL DEFAULT_PAN_ID=$PAN_ID

make -C ./br/gnrc_border_router ETHOS_BAUDRATE=500000 BOARD=$BOARD DEFAULT_CHANNEL=$CHANNEL DEFAULT_PAN_ID=$PAN_ID #IOTLAB_NODE=m3-${BR_ID}.${SITE}.iot-lab.info flash

if [ -n "$IOT_LAB_FRONTEND_FQDN" ]; then
  # Make sure that the monitoring profile exists
  iotlab-profile del -n monitor_sensors_m3
  iotlab-profile addm3 -n monitor_sensors_m3 -voltage -current -power -period 8244 -avg 16

  # Flash the border router
  echo "Flashing Border Router"
  cp br/gnrc_border_router/bin/${BOARD}/gnrc_border_router.elf ~/shared/
  iotlab-experiment submit -n iot2023oulu_br -d 60 -l ${SITE},${ARCH},${BR_ID},~/shared/gnrc_border_router.elf,monitor_sensors_m3
  iotlab-experiment wait --timeout 30 --cancel-on-timeout

  # Flash the sensor nodes
  echo "Flashing Sensor Nodes"
  cp measurer/bin/${BOARD}/iot2023sensor.elf ~/shared/
  iotlab-experiment submit -n iot2023oulu -d 60 -l ${SITE},${ARCH},${SENSORS},~/shared/iot2023sensor.elf,monitor_sensors_m3
  iotlab-experiment wait --timeout 30 --cancel-on-timeout

  # Start ethos on the border router
  echo "Starting Ethos BR:${BR_ID}, TAP:${TAP}, IPV6:${IPV6}::/64"
  sudo ethos_uhcpd.py m3-${BR_ID} tap${TAP} ${IPV6}::/64

fi
