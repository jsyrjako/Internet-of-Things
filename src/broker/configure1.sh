#!/bin/bash

# Check if a parameter has been provided
# if [ -z "$1" ]
# then
#     echo "Usage: ./configure.sh <ssh_user>"
#     exit 1
# fi

# Define the ssh_user and create target_node
# ssh_user=$1

# iotlab-auth -u $ssh_user
iotlab-experiment submit -n riot_mqtt -d 60 -l 1,archi=a8:at86rf231+site=grenoble
iotlab-experiment wait
#iotlab-ssh --verbose wait-for-boot #causes proxy auth error using this script -> use sleep instead

target_address=$(iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes | tr -d '"[] ')
target=$(echo $target_address | cut -d'.' -f1)
target_node="node-$target"

echo "Waiting 45s for the node to boot..."
sleep 45s
scp ./config.conf root@$target_node:~/
ssh -T root@$target_node << EOF
ip -6 -o addr show eth0 | grep 'global'

broker_mqtts config.conf
EOF

# Print the global IPv6 address of the node


# Start the MQTT broker

