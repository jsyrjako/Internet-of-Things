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

int main(void)
{
    /* we need a message queue for the thread running the shell in order to
     * receive potentially fast incoming networking packets */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);
    puts("RIOT network stack example application");

    // if (init_sensors() == 0)
    // {
    //     thread_create(stack_size, sizeof(stack_size), THREAD_PRIORITY_MAIN - 1, 0, sensor_thread, NULL, "sensor_thread");
    // }

    /* start shell */
    puts("All up, running the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}
