/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Example application for demonstrating the RIOT network stack
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>

#include "ztimer.h"
#include "thread.h"
#include "shell.h"
#include "msg.h"

#include "lpsxxx.h"
#include "lpsxxx_params.h"
#include "isl29020.h"
#include "isl29020_params.h"

#include "net/gcoap.h"
#include "net/sock/udp.h"
#include "net/ipv6/addr.h"


/* CoAP resources */
// static ssize_t _encode_link(const coap_resource_t *resource, char *buf,
//                             size_t maxlen, coap_link_encoder_ctx_t *context);
static ssize_t _sensor_data_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, void *ctx);

/* CoAP resource list */
static const coap_resource_t _resources[] = {
    { "/sensor/data", COAP_GET, _sensor_data_handler, NULL },
};

/* CoAP server */
static gcoap_listener_t _listener = {
    &_resources[0],
    ARRAY_SIZE(_resources),
    gcoap_encode_link,
    NULL};

/* Adds link format params to resource list */
// static ssize_t _encode_link(const coap_resource_t *resource, char *buf,
//                             size_t maxlen, coap_link_encoder_ctx_t *context)
// {
//     ssize_t res = gcoap_encode_link(resource, buf, maxlen, context);

//     return res;
// }

void gcoap_cli_init(void)
{
    gcoap_register_listener(&_listener);
}

extern int gcoap_cli_cmd(int argc, char **argv);
extern void gcoap_cli_init(void);


// UDP
#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

extern int udp_cmd(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "udp", "send data over UDP and listen on UDP ports", udp_cmd },
    { NULL, NULL, NULL }
};

// Sensing
static lpsxxx_params_t lps_params = {
    .i2c = lpsxxx_params[0].i2c,
    .addr = lpsxxx_params[0].addr,
    .rate = LPSXXX_RATE_1HZ,
};

static isl29020_params_t isl_params = {
    .i2c = isl29020_params[0].i2c,
    .addr = isl29020_params[0].addr,
    .mode = ISL29020_MODE_AMBIENT,
    .range = ISL29020_RANGE_1K,
};

static lpsxxx_t lpsxxx;
static isl29020_t isl29020;

static char stack_size[THREAD_STACKSIZE_DEFAULT];

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

static ssize_t _sensor_data_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, void *ctx)
{
    (void)pdu;
    (void)ctx;
    char message[32];
    int16_t temp = read_temperature();
    uint16_t pres = read_pressure();
    int light = read_light();
    snprintf(message, sizeof(message), "Temperature: %i.%u°C\nPressure: %uhPa\nLight: %d", (temp / 100), (temp % 100), pres, light);
    return gcoap_response(pdu, buf, len, COAP_CODE_CONTENT);
}

void _resp_handler(const gcoap_request_memo_t *memo, coap_pkt_t* pdu, const sock_udp_ep_t *remote) {
    (void)remote;

    if (memo->state == GCOAP_MEMO_RESP) {
        char *payload = (char *)pdu->payload;
        printf("Response: %s\n", payload);
    } else {
        printf("Request failed\n");
    }
}

void send_sensor_data(int16_t temp, uint16_t pres, int light)
{
   /* Format sensor data into a string */
    char message[64];
    snprintf(message, sizeof(message), "Temperature: %i.%u°C\nPressure: %uhPa\nLight: %d", (temp / 100), (temp % 100), pres, light);

    /* Create a CoAP packet */
    coap_pkt_t pdu;
    uint8_t buf[CONFIG_GCOAP_PDU_BUF_SIZE];
    gcoap_req_init(&pdu, buf, CONFIG_GCOAP_PDU_BUF_SIZE, COAP_METHOD_GET, "/sensor/data");

    /* Set the payload of the CoAP packet */
    size_t payload_len = strlen(message);
    pdu.payload_len = payload_len;
    memcpy(pdu.payload, message, payload_len);

     /* Specify the remote endpoint */
    sock_udp_ep_t remote;
    // remote.family = AF_INET6;
    remote.port = 5683;
    ipv6_addr_from_str((ipv6_addr_t *)&remote.addr.ipv6, "2001:660:4403:496:ac5a:fa07:a24f:9ec3");  // Replace with your destination address

    /* Send the CoAP packet */
    gcoap_req_send(buf, pdu.payload_len, &remote, _resp_handler, NULL);
}

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

        send_sensor_data(temp, pres, light);
        printf("Sensor data sent\n");

        ztimer_sleep(ZTIMER_MSEC, 5000);
    }
    return 0;
}

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT network stack example application");

    if (init_sensors() == 0)
    {
        thread_create(stack_size, sizeof(stack_size), THREAD_PRIORITY_MAIN - 1, 0, sensor_thread, NULL, "sensor_thread");
    }

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
