#!/bin/bash

BOARD=iotlab-m3
BR_ID=180
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

# iotlab-experiment submit -n "sensors_ipv6" -d 120 -l archi=m3:at86rf231+site=$SITE
# iotlab-experiment wait --timeout 30 --cancel-on-timeout

# Make the sensor nodes
make -C ./measurer/ BOARD=$BOARD DEFAULT_CHANNEL=$CHANNEL DEFAULT_PAN_ID=$PAN_ID

make -C ./br/gnrc_border_router ETHOS_BAUDRATE=500000 BOARD=$BOARD DEFAULT_CHANNEL=$CHANNEL DEFAULT_PAN_ID=$PAN_ID IOTLAB_NODE=m3-${BR_ID}.${SITE}.iot-lab.info flash

if [ -n "$IOT_LAB_FRONTEND_FQDN" ]; then
  # Make sure that the monitoring profile exists
  iotlab-profile del -n monitor_sensors_m3
  iotlab-profile addm3 -n monitor_sensors_m3 -voltage -current -power -period 8244 -avg 16

  # Flash the border router
  echo "Flashing Border Router"
  cp br/gnrc_border_router/bin/${ARCH}/gnrc_border_router.elf ~/shared/
  iotlab-experiment submit -n iot2023oulu_br -d 60 -l ${SITE},m3,${BR_ID},~/shared/gnrc_border_router.elf,monitor_sensors_m3
  iotlab-experiment wait --timeout 30 --cancel-on-timeout

  # Flash the sensor nodes
  echo "Flashing Sensor Nodes"
  cp measurer/bin/${ARCH}/iot2023sensor.elf ~/shared/
  iotlab-experiment submit -n iot2023oulu -d 60 -l ${SITE},m3,${SENSORS},~/shared/iot2023sensor.elf,monitor_sensors_m3
  iotlab-experiment wait --timeout 30 --cancel-on-timeout

  # Start ethos on the border router
  sudo ethos_uhcpd.py m3-180 tap69 2001:660:5307:3169::/64

fi
