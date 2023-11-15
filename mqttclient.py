import paho.mqtt.client as mqtt


# set username and password for iot-lab MQTT broker
username = "your_username"
password = "your_password"
# set path to CA certificate file for iot-lab MQTT broker
ca_cert_path = "./iot-lab-ca.pem"


# Define callback functions for MQTT events
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("iotlab/<iot2023oulu21>/test")


def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))


# Create MQTT client instance
client = mqtt.Client()

# Set TLS options
client.tls_set(ca_certs=ca_cert_path)

# Set username and password
client.username_pw_set(username, password)

# Set callback functions
client.on_connect = on_connect
client.on_message = on_message

# Connect to broker
client.connect("mqtt.iot-lab.info", 8883)

# Start loop to process incoming messages
client.loop_forever()
