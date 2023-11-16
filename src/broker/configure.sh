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
iotlab-ssh --verbose wait-for-boot

target_node=$(iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes)

ssh root@$target_node << EOF

# Create and edit the config.conf file
cat << EOT > config.conf
# add some debug output
trace_output protocol

# listen for MQTT-SN traffic on UDP port 1885
listener 1885 INADDR_ANY mqtts
  ipv6 true

# listen to MQTT connections on tcp port 1886
listener 1886 INADDR_ANY
  ipv6 true
EOT

# Print the global IPv6 address of the node
ip -6 -o addr show eth0 | grep 'global'

# Start the MQTT broker
broker_mqtts config.conf

EOF
