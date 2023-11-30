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

#include "coap_client.h"

#define MOVING_BUFFER_SIZE 6

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

static int16_t temp_buffer[MOVING_BUFFER_SIZE] = {0};
static uint16_t pres_buffer[MOVING_BUFFER_SIZE] = {0};
static int light_buffer[MOVING_BUFFER_SIZE] = {0};
static int temp_buffer_index = 0;
static int pres_buffer_index = 0;
static int light_buffer_index = 0;

void temp_to_buffer(int16_t temp)
{
    temp_buffer[temp_buffer_index] = temp;
    temp_buffer_index = (temp_buffer_index + 1) % MOVING_BUFFER_SIZE;
}

void pres_to_buffer(uint16_t pres)
{
    pres_buffer[pres_buffer_index] = pres;
    pres_buffer_index = (pres_buffer_index + 1) % MOVING_BUFFER_SIZE;
}

void light_to_buffer(int light)
{
    light_buffer[light_buffer_index] = light;
    light_buffer_index = (light_buffer_index + 1) % MOVING_BUFFER_SIZE;
}

void calculate_average_temp(int16_t *avg_temp)
{
    int16_t sum_temp = 0;

    for (int i = 0; i < MOVING_BUFFER_SIZE; i++)
    {
        sum_temp += temp_buffer[i];
    }

    *avg_temp = sum_temp / MOVING_BUFFER_SIZE;
}

void calculate_average_pres(uint16_t *avg_pres)
{
    uint16_t sum_pres = 0;

    for (int i = 0; i < MOVING_BUFFER_SIZE; i++)
    {
        sum_pres += pres_buffer[i];
    }

    *avg_pres = sum_pres / MOVING_BUFFER_SIZE;
}

void calculate_average_light(int *avg_light)
{
    int sum_light = 0;

    for (int i = 0; i < MOVING_BUFFER_SIZE; i++)
    {
        sum_light += light_buffer[i];
    }

    *avg_light = sum_light / MOVING_BUFFER_SIZE;
}

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
        perror("Failed to initialize sensor ISL29020");
        return 2;
    }
    // Sleep
    ztimer_sleep(ZTIMER_MSEC, 1000);
    return 0;
}

int16_t read_temperature(void)
{
    int16_t temp;
    if (lpsxxx_read_temp(&lpsxxx, &temp) != LPSXXX_OK)
    {
        perror("Failed to read temperature");
    }
    else
    {
        temp_to_buffer(temp);
    }
    return temp;
}

uint16_t read_pressure(void)
{
    uint16_t pres;
    if (lpsxxx_read_pres(&lpsxxx, &pres) != LPSXXX_OK)
    {
        perror("Failed to read pressure");
    }
    else
    {
        pres_to_buffer(pres);
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
    else
    {
        light_to_buffer(light);
    }
    return light;
}

static void *sensor_thread(void *arg)
{
    (void)arg;
    int16_t temp, avg_temp;
    uint16_t pres, avg_pres;
    int light, avg_light;
    char tmsg[31], pmsg[31], lmsg[31];

    // Call init_addresses to initialize the server address and node ID
    init_addresses();

    while (1)
    {
        temp = read_temperature();
        pres = read_pressure();
        light = read_light();
        printf("Temperature: %i.%u °C, Pressure: %u hPa, Light: %d lux\n", (temp / 100), (temp % 100), pres, light);

        if (temp_buffer_index == 5 )
        {
            calculate_average_temp(&avg_temp);
            printf("Avg Temp: %i.%u °C, \n", (avg_temp / 100), (avg_temp % 100));
            sprintf(tmsg, "\"temperature\":\"%i.%u\"", (avg_temp / 100), (avg_temp % 100));
            send_to_coap_server(tmsg);
        }
        if (pres_buffer_index == 5)
        {
            calculate_average_pres(&avg_pres);
            printf("Avg Pres: %u hPa\n", avg_pres);
            sprintf(pmsg, "\"pressure\":\"%u\"", avg_pres);
            send_to_coap_server(pmsg);
        }
        if (light_buffer_index == 5)
        {
            calculate_average_light(&avg_light);
            printf("Avg Light: %d lux\n", avg_light);
            sprintf(lmsg, "\"light\":\"%d\"", avg_light);
            send_to_coap_server(lmsg);
        }
        ztimer_sleep(ZTIMER_MSEC, 5000);
    }
    return 0;
}

int main(void)
{
    if (init_sensors() == 0)
    {
        thread_create(stack_size, sizeof(stack_size), THREAD_PRIORITY_MAIN - 1, 0, sensor_thread, NULL, "sensor_thread");
    }

    return 0;
}
