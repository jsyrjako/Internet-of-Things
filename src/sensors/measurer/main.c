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

    while (1)
    {
        temp = read_temperature();
        pres = read_pressure();
        light = read_light();
        printf("Temperature: %i.%u °C, Pressure: %u hPa, Light: %d lux\n", (temp / 100), (temp % 100), pres, light);

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
