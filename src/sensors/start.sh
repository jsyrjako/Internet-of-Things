#!/bin/bash

COAP_SERVER_IP=2001:660:5307::      # CoAP server IPv6 address  <- Change this to your own
BOARD=iotlab-m3                     # Board architecture        iotlab-m3|iotlab-a8-m3|iotlab-a8
NODES=180-185                       # Sensor node IDs           site specific
BR_ID=180                           # Border router node ID     site specific
IPV6=2001:660:5307:3169             # IPv6 prefix               site specific
TAP=69                              # TAP interface number      0-255
SENSORS=181-185                     # Sensor node IDs           1-10
SITE=grenoble                       # IoT-LAB site              lille|grenoble|saclay|strasbourg
PAN_ID=0xface                       # 802.15.4 PAN ID           0x0000-0xffff
CHANNEL=16                          # 802.15.4 channel          11-26
ARCH=m3                             # Board architecture

if [ -n "$IOT_LAB_FRONTEND_FQDN" ]; then
  # RIOT doesnt support the arm gcc version installed by default on the SSH frontend, e.g. 4.9.
  # Therefore, we use arm gcc version 9 found in /opt/
  source /opt/riot.source
else
  echo "[ERROR] Not in IoT-LAB frontend"
fi

if [ -n "$IOT_LAB_FRONTEND_FQDN" ]; then
  # Start experiment
  iotlab-experiment submit -n "sensors_ipv6" -d 240 -l ${SITE},${ARCH},${NODES}
  iotlab-experiment wait --timeout 30 --cancel-on-timeout

  # Make sure that the monitoring profile exists
  iotlab-profile del -n monitor_sensors_m3
  iotlab-profile addm3 -n monitor_sensors_m3 -voltage -current -power -period 8244 -avg 16
  iotlab-node --update-profile monitor_sensors_m3

  # Make the sensor nodes
  make -C ./measurer/ BOARD=$BOARD DEFAULT_CHANNEL=$CHANNEL DEFAULT_PAN_ID=$PAN_ID COAP_SERVER_IPV6_ADDR=${COAP_SERVER_IP}
  echo "Flashing Sensor Nodes"
  cp measurer/bin/${BOARD}/iot2023sensor.elf ~/shared/
  iotlab-node --flash ~/shared/iot2023sensor.elf -l ${SITE},${ARCH},${SENSORS}
  sleep 10

  make -C ./br/gnrc_border_router ETHOS_BAUDRATE=500000 BOARD=$BOARD DEFAULT_CHANNEL=$CHANNEL DEFAULT_PAN_ID=$PAN_ID IOTLAB_NODE=m3-${BR_ID}.${SITE}.iot-lab.info flash

  # Start ethos on the border router
  echo "Starting Ethos BR:${BR_ID}, TAP:${TAP}, IPV6:${IPV6}::/64"
  sudo ethos_uhcpd.py m3-${BR_ID} tap${TAP} ${IPV6}::/64

fi
