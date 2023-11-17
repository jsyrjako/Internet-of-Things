#!/bin/bash

# Check if a parameter has been provided
if [ -z "$1" ]
then
    echo "Usage: ./configure.sh <ssh_user>"
    exit 1
fi

# Define the ssh_user and create target_node
ssh_user=$1

iotlab-auth -u $ssh_user
iotlab-experiment submit -n riot_mqtt -d 60 -l 1,archi=a8:at86rf231+site=grenoble
iotlab-experiment wait
# iotlab-ssh --verbose wait-for-boot

target_node=$(iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes)
target=${target_node[0]}

echo $target
scp ./config.conf root@$target:~/
ssh root@$target

# Print the global IPv6 address of the node
ip -6 -o addr show eth0 | grep 'global'

# Start the MQTT broker
broker_mqtts config.conf
