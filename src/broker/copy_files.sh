#!/bin/bash

# Check if a parameter has been provided
if [ -z "$1" ]
then
    echo "Usage: ./configure.sh <ssh_user>"
    exit 1
fi

# Define the ssh_user and create target_node
ssh_user=$1

scp ./configure1.sh $ssh_user@grenoble.iot-lab.info:~/
scp ./config.conf $ssh_user@grenoble.iot-lab.info:~/
ssh $ssh_user@grenoble.iot-lab.info
