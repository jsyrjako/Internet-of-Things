#!/bin/bash

# Check if a parameter has been provided
if [ -z "$1" ]
then
    echo "Usage: ./configure.sh <>"
    exit 1
fi

iotlab-experiment submit -n border_router -d 60 -l 1,archi=a8:at86rf231+site=grenoble
iotlab-experiment wait

# Get the code of the 2019.01 release of RIOT from GitHub
mkdir -p ~/A8/riot
cd ~/A8/riot
git clone https://github.com/RIOT-OS/RIOT.git -b 2019.01-branch
cd RIOT

# Build and flash the required firmware for the border router node
source /opt/riot.source
make ETHOS_BAUDRATE=500000 BOARD=iotlab-a8-m3 clean all -C examples/gnrc_border_router
iotlab-ssh flash-m3 examples/gnrc_border_router/bin/iotlab-a8-m3/gnrc_border_router.elf -l saclay,a8,1

# Get the address of the border router node
target_address=$(iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes | tr -d '"[] ')
target=$(echo $target_address | cut -d'.' -f1)
target_node="node-$target"
echo "Target node:" $target_node

# Connect to the border router node
ssh -T root@$target_node << EOF
cd ~/A8/riot/RIOT/dist/tools/uhcpd
make clean all
cd ../ethos
make clean all

# Start the network
./start_network.sh /dev/ttyA8_M3 tap0 2001:660:3207:401::/64 500000
net.ipv6.conf.tap0.forwarding = 1
net.ipv6.conf.tap0.accept_ra = 0

# Get the information of the border router node
cd\
printenv

EOF
