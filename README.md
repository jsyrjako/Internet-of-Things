# Internet-of-Things

Repository for 521043S-3004 Internet of Things 2023 Mini Project 1

Made by:
| Name              | Student ID    | Email                           |
| ----------------- | ------------- | ------------------------------- |
| Janne Yrjänäinen  | Y58554010      | jyrjanai20@student.oulu.fi      |
| Joona Syrjäkoski  | Y58172265      | jsyrjako20@student.oulu.fi      |


## Instructions:

### How to run the project on IoT-Lab:

1. Login to FIT IoT-Lab by using SSH:

   - `ssh <username>@<site>.iot-lab.info`

2. Clone the repository:

   - `git clone https://github.com/jsyrjako/Internet-of-Things.git`

3. Change directory:

   - `cd Internet-of-Things`

4. Initialize submodules:

   - `git submodule update --init`

5. Run the project:
   #### Start the CoAP server:
    - `cd src/coap-server/`
    - Install & Activate venv according to src/coap-server/README.md
    - Start server with `python coap-server.py`

    #### Start the Sensor nodes:
    - Clone the repository to IoT-Lab site
    - If you don’t want to specify your credentials everytime you launch a command, run `iotlab-auth -u <login>` to store your credentials.
    - `cd src/sensors/`
    - Edit `./start.sh`
        - Match CoAP server IPv6 address
        - Check for free tap interface number `ip addr show | grep tap` and change if needed
        - Check for free IPv6 address prefix for the Border router with `ip -6 route` and change if needed
        - Change the node numbers to be flashed based on the site and available nodes [here](https://iot-lab.github.io/docs/deployment/grenoble/)

    - Run start script with `./start.sh`
    - The script will launch an experiment on IoT-Lab, flash one of the nodes with the border router firmware and the rest with the sensor firmware.

## Project description:

### [IoT-Lab M3](https://iot-lab.github.io/docs/boards/iot-lab-m3/)

![Image of IoT-Lab M3](./images/m3-impl.png)

We are using IoT-Lab M3 nodes to measure temperature, pressure and luminosity.

Sensors used:

- Light: ISL29020
- Temperature: LPS331AP
- Pressure: LPS331AP

![Architecture of the M3](./images/m3-architecture.png)

Sensors are connected to the M3 via I2C bus. Measurements are read from the sensors in 5 second cycles. The sensor nodes will calculate a moving average of the last 6 measurements and send the average to the CoAP Server through border router via the 6LoWPAN network.

## Set Up EC2 Instance and Assign IPv6 Address

1. Create EC2 instance

   - [Amazon Docs](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/EC2_GetStarted.html)

2. Create and attach IPv6 address to EC2 instance

   - [Amazon Docs](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/using-instance-addressing.html#working-with-ipv6-addresses)

3. Allow CoAP traffic to EC2 instance
   - Allow UDP traffic on port 5683
