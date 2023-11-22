#include <stdio.h>
#include <stdlib.h>
#include "ztimer.h"
#include "MQTTClient.h"

#include "lpsxxx.h"
#include "lpsxxx_params.h"

#include "isl29020.h"

// Get node's ID
char* node_id = get_node_id();
char* CLIENTID = node_id;

// Initialize MQTTClient
MQTTClient client;
static const char *URL = "<mqtt node's address>:1886";
MQTTClient_create(&client, URL, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

// Connect to MQTTClient
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;

// conn_opts.keepAliveInterval = 10;
// conn_opts.cleansession = 1;
// conn_opts.username = USERNAME;
// conn_opts.password = PASSWORD;
MQTTClient_connect(client, &conn_opts);
printf("MQTT client connected\n");


// Set up variables
char topic[256];
sprintf(topic, "sensor/data/%s", node_id);
static const int QOS = 1;
// MQTTClient_deliveryToken token;

// Sensors
static lpsxxx_t lpsxxx;
static isl29020_t isl29020;

int get_node_id()
{
    char hostname[1024];
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
    printf("Hostname: %s\n", hostname);
    struct hostent* h;
    h = gethostbyname(hostname);
    printf("Hostent: %s\n", h->h_name);
    char* node_id = h->h_name;
    return node_id;
}


int init_sensors(void)
{
    // Initialize LPS331AP sensor
    if (lpsxxx_init(&lpsxxx, &lpsxxx_params[0]) != LPSXXX_OK)
    {
        perror("Failed to initialize sensor LPS331AP");
    }

    // Initialize ISL29020 sensor
    if (isl29020_init(&isl29020, &isl29020_params[0]) != 0)
    {
        perror("Failed to initialize sensor LPS331AP");
    }
}

int read_temperature()
{
    uint16_t temp;
    if (lpsxxx_read_temp(&lpsxxx, &temp) != LPSXXX_OK)
    {
        perror("Failed to read temperature");
    }
    else
    {
        printf("Temperature: %i.%u°C\n", (temp / 100), (temp % 100));
    }
    ztimer_sleep(ZTIMER_MSEC, 500);
    return temp;
}

int read_pressure()
{
    uint16_t pres;
    if (lpsxxx_read_pres(&pres) != LPSXXX_OK)
    {
        perror("Failed to read pressure");
    }
    else
    {
        printf("Pressure: %uhPa\n", pres);
    }
    ztimer_sleep(ZTIMER_MSEC, 500);
    return pres;
}

int read_light()
{
    uint16_t light = isl29020_read(&isl29020);
    if (light == -1)
    {
        perror("Failed to read light");
    }
    else
    {
        printf("Light: %d\n", light);
    }
    ztimer_sleep(ZTIMER_MSEC, 500);
    return light;
}

char* create_mqtt_message(uint16_t temp, uint16_t pres, uint16_t light)
{
    char* message = malloc(100);
    sprintf(message, "{\"temperature\": %i.%u, \"pressure\": %uhPa, \"light\": %d}", (temp), (temp), pres, light);
    printf("Message: %s\n", message);
    return message;
}

void publish_message(char* message)
{
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = message;
    pubmsg.payloadlen = strlen(message);
    pubmsg.qos = QOS;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    printf("Message published\n");
}

int main(void)
{
    init_sensors();
    uint16_t temp, pres, light;
    char* message;

    while (1)
    {
        temp = read_temperature();
        pres = read_pressure();
        light = read_light();
        message = create_mqtt_message(temp, pres, light);
        publish_message(message);
        ztimer_sleep(ZTIMER_MSEC, 5000);
    }

    return 0;
}
