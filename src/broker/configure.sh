#!/bin/bash

# Check if a parameter has been provided
if [ -z "$1" ]
then
    echo "No SSH user provided. Usage: ./configure.sh <ssh_user>"
    exit 1
fi

# Define the target server and the path to the config.conf file
ssh_user=$1
target_server=$ssh_user@grenoble.iot-lab.info
config_file_path=./config.conf

# Copy the config.conf file to the target server
scp $config_file_path $target_server:/path/to/destination

# Print out the global IPv6 address of the target server
ssh $target_server "ip -6 addr | grep 'global' | awk '{print $2}'"

ssh $target_server "broker_mqtts config.conf"
