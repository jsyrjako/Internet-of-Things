import paho.mqtt.client as mqtt

# set username and password for iot-lab MQTT broker
username = "your_username"
password = "your_password"
# set path to CA certificate file for iot-lab MQTT broker
ca_cert_path = "./iot-lab-ca.pem"


# AWS dynamoDB
dynamoDB = boto3.client('dynamodb')
table_name = "your_table_name"


# Define callback functions for MQTT events
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("iotlab/<iot2023oulu21>/test")


# Save MQTT message to database
def on_message(msg):
    print(msg.topic + " " + str(msg.payload))

    # Extract MQTT message from payload
    mqtt_msg = json.loads(msg.payload)

    # Insert MQTT message to database
    responce = dynamoDB.put_item(
        TableName=table_name,
        Item={
            'timestamp': {'S': mqtt_msg['timestamp']},
            'temperature': {'N': mqtt_msg['temperature']},
            'pressure': {'N': mqtt_msg['pressure']},
            'light': {'N': mqtt_msg['light']},
        }
    )
    print(responce)





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
