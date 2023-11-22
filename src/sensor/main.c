#include <stdio.h>
#include <stdlib.h>
#include "ztimer.h"
#include "thread.h"
// #include "paho_mqtt.h"
// #include "MQTTClient.h"

#include "lpsxxx.h"
#include "lpsxxx_params.h"

#include "isl29020.h"
#include "isl29020_params.h"

// #define URL "ssl://eu2.cloud.thethings.industries:1883"
// #define CLIENTID "1"
// #define TOPIC "devices/1/data/"
// #define QOS 1
// #define USERNAME "iot-2023@di-ttn-iot-2023"
// #define PASSWORD "NNSXS.DHBYTCSWOJUZHSF2VR6RDOUPVGSP2LKGX4N5ZKY.4D4JR5UZGOPRNLVYZ3ADTWFU4MYYQZUPBEXHABCRWGSVUV5MVAZQ"

// Set up variables
// char topic[256];
// sprintf(topic, "devices/%s/data/", CLIENTID);
// static const int QOS = 1;

// MQTTCLIENT
// #define MQTTCLIENT_SUCCESS 0
// #define MQTTCLIENT_FAILURE -1
// #define MQTTCLIENT_DISCONNECTED -3

/* // #define MQTTVERSION_DEFAULT 0
// #define MQTTProperties_initializer \
//     {                              \
//         0, 0, 0, NULL              \
//     }
// #define MQTTClient_connectOptions_initializer                                                                                          \
//     {                                                                                                                                  \
//         {'M', 'Q', 'T', 'C'}, 6, 60, 1, 1, NULL, NULL, NULL, 30, 0, NULL, 0, NULL, MQTTVERSION_DEFAULT, {NULL, 0, 0}, {0, NULL}, -1, 0 \
//     }
// #define MQTTClient_message_initializer                                           \
//     {                                                                            \
//         {'M', 'Q', 'T', 'M'}, 1, 0, NULL, 0, 0, 0, 0, MQTTProperties_initializer \
//     }
// #define MQTTCLIENT_PERSISTENCE_NONE   1 */

// Sensor parameters
static lpsxxx_params_t lps_params = {
    .i2c = lpsxxx_params[0].i2c,
    .addr = lpsxxx_params[0].addr,
    .rate = LPSXXX_RATE_1HZ,
};

// Sensor parameters
static isl29020_params_t isl_params = {
    .i2c = isl29020_params[0].i2c,
    .addr = isl29020_params[0].addr,
    .mode = ISL29020_MODE_AMBIENT,
    .range = ISL29020_RANGE_1K,
};

// Sensors
static lpsxxx_t lpsxxx;
static isl29020_t isl29020;

static char stack_size[THREAD_STACKSIZE_DEFAULT];

// int get_node_id()
// {
//     char hostname[1024];
//     hostname[1023] = '\0';
//     gethostname(hostname, 1023);
//     printf("Hostname: %s\n", hostname);
//     struct hostent *h;
//     h = gethostbyname(hostname);
//     printf("Hostent: %s\n", h->h_name);
//     char *node_id = h->h_name;
//     return node_id;
// }

int init_sensors(void)
{
    // Initialize LPS331AP sensor
    if (lpsxxx_init(&lpsxxx, &lps_params) != LPSXXX_OK)
    {
        perror("Failed to initialize sensor LPS331AP");
        return 1;
    }

    // Initialize ISL29020 sensor
    if (isl29020_init(&isl29020, &isl_params) != 0)
    {
        perror("Failed to initialize sensor LPS331AP");
        return 2;
    }
    return 0;
}

int read_temperature(void)
{
    int16_t temp;
    if (lpsxxx_read_temp(&lpsxxx, &temp) != LPSXXX_OK)
    {
        perror("Failed to read temperature");
    }
    return temp;
}

int read_pressure(void)
{
    uint16_t pres;
    if (lpsxxx_read_pres(&lpsxxx, &pres) != LPSXXX_OK)
    {
        perror("Failed to read pressure");
    }
    return pres;
}

int read_light(void)
{
    int light = isl29020_read(&isl29020);
    if (light == -1)
    {
        perror("Failed to read light");
    }
    return light;
}

// char *create_mqtt_message(uint16_t temp, uint16_t pres, uint16_t light)
// {
//     char *message = malloc(100);
//     sprintf(message, "{\"temperature\": %i.%u, \"pressure\": %uhPa, \"light\": %d}", (temp), (temp), pres, light);
//     printf("Message: %s\n", message);
//     return message;
// }

// void publish_message(char *message)
// {
//     // const char* topic = "devices/1/data/";
//     // MQTTClient_subscribe(client, topic, qos);
//     // printf("subscribed to %s \n", topic);

//     // // payload the content of your message
//     // char* payload = message;
//     // int payloadlen = strlen(payload);
//     // MQTTClient_deliveryToken dt;
//     // MQTTClient_publish(client, topic, payloadlen, payload, qos, retained, &dt);
//     // printf("published to %s \n", topic);

//     int rc;
//     MQTTClient_message pubmsg = MQTTClient_message_initializer;
//     MQTTClient_deliveryToken token;
//     pubmsg.payload = message;
//     pubmsg.payloadlen = strlen(message);
//     pubmsg.qos = QOS;
//     pubmsg.retained = 0;

//     MQTTClient_publishMessage(client, TOPIC, &pubmsg, &token);
//     printf("Waiting for up to %d seconds for publication of %s\n"
//            "on topic %s for client with ClientID: %s\n",
//            (int)(TIMEOUT / 1000), PAYLOAD, TOPIC, CLIENTID);
//     rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
//     printf("Message with delivery token %d delivered\n", token);
// }

static void *sensor_thread(void *arg)
{
    (void)arg;
    int16_t temp;
    uint16_t pres;
    int light;

    // char *message;
    while (1)
    {
        temp = read_temperature();
        pres = read_pressure();
        light = read_light();
        printf("Temperature: %i.%u°C\n", (temp / 100), (temp % 100));
        printf("Pressure: %uhPa\n", pres);
        printf("Light: %d\n", light);
        // message = create_mqtt_message(temp, pres, light);
        // publish_message(message);
        ztimer_sleep(ZTIMER_MSEC, 5000);
    }
    return 0;
}

// Connect to MQTTClient
// void connect_mqtt()
// {
//     MQTTClient client;
//     MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
//     int rc;
//     // conn_opts.keepAliveInterval = 10;
//     // conn_opts.cleansession = 1;
//     conn_opts.username = USERNAME;
//     conn_opts.password = PASSWORD;

//     MQTTClient_create(&client, URL, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
//     if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
//     {
//         printf("Failed to connect, return code %d\n", rc);
//         exit(EXIT_FAILURE);
//     }
// }

int main(void)
{
    // connect_mqtt();

    if (init_sensors() == 0)
    {
        thread_create(stack_size, sizeof(stack_size), THREAD_PRIORITY_MAIN - 1, 0, sensor_thread, NULL, "sensor_thread");
    }

    return 0;
}
