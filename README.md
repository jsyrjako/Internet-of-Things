# Internet-of-Things
521043S-3004 Internet of Things 2023

## Instructions:

### How to run the project on IoT-Lab:
1. Login to FIT IoT-Lab by using SSH:
    - ```ssh <username>@<site>.iot-lab.info```

2. Clone the repository:
    - ```git clone https://github.com/jsyrjako/Internet-of-Things.git```

3. Change directory:
    - ```cd Internet-of-Things```

4. Initialize submodules:
    - ```git submodule update --init```

5. Run the project:
    - ```tähän koodia :D```
    - ```iotlab-experiment submit -n sensor -d 120 -l 1,archi=m3:at86rf231+site=grenoble```
    - ```iotlab-experiment wait --timeout 30 --cancel-on-timeout```
    - ```iotlab-experiment --jmespath="items[*].network_address | sort(@)" get --nodes```
    - ```make BOARD=iotlab-m3 IOTLAB_NODE=auto flash term```

## Project description:

### [IoT-Lab M3](https://iot-lab.github.io/docs/boards/iot-lab-m3/)
![Image of IoT-Lab M3](./images/m3-impl.png)

We are using IoT-Lab M3 nodes to measure temperature, pressure and luminosity.

Sensors used:
- Light: ISL29020
- Temperature: LPS331AP
- Pressure: LPS331AP

![Architecture of the M3](./images/m3-architecture.png)


## Set Up EC2 Instance and Assign IPv6 Address

1. Create EC2 instance
    - [Amazon Docs](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/EC2_GetStarted.html)

2. Create and attach IPv6 address to EC2 instance
    - [Amazon Docs](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/using-instance-addressing.html#working-with-ipv6-addresses)

3. Allow CoAP traffic to EC2 instance
    - Allow UDP traffic on port 5683
