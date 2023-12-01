# Sensors

## Sensor Usage and Average Calculation

### ISL29020 Sensor

The ISL29020 sensor is a digital light sensor that measures ambient light intensity. It provides accurate light intensity readings in lux. How the ISL29020 sensor is used:

1. Initialize the sensor.
2. Read the light intensity value from the sensor.
3. Calculate the moving average of 6 measurements.

Datasheet for the ISL29020 sensor can be found [here](https://iot-lab.github.io/assets/misc/docs/iot-lab-m3/ISL29020.pdf).

### LPS331AP Sensor

The LPS331AP sensor is a digital barometer and altimeter that measures atmospheric pressure. It provides accurate pressure and temperature readings. How the LPS331AP sensor is used:

1. Initialize the sensor.
2. Read the pressure value from the sensor.
3. Read the temperature value from the sensor.
4. Calculate the average of 6 measurements.

Datasheet for the LPS331AP sensor can be found [here](https://iot-lab.github.io/assets/misc/docs/iot-lab-m3/LPS331AP.pdf).

### Average Calculation

The calculation of the moving average is done in the following way:

1. Collect measurement and store it in a buffer.
2. Move the buffer index to the next position.
3. If the buffer is full, move the buffer index to the first position.
4. Calculate the average of the buffer by summing up all the measurements and dividing the sum by the number of measurements (in this case, six).

## Sending Data to the CoAP Server

### CoAP

The data is packaged into a CoAP request. This involves creating a CoAP packet and adding the processed data to the payload of the packet. The CoAP request is sent to the CoAP server. After the CoAP request has been sent, the sensor node waits for a response from the CoAP server. When a response is received, the sensor node handles the response appropriately.
