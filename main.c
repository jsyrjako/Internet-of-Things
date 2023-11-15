#include <stdio.h>
#include <stdlib.h>
#include "ztimer.h"

#include "lpsxxx.h"
#include "lpsxxx_params.h"

#include "isl29020.h"

// MQTT message parameters
#define MSG_LEN (1040u)
#define MSG "{ 'temperature': %d, 'pressure': %d, 'light': %d }"

// Sensors
static lpsxxx_t lpsxxx;
static isl29020_t isl29020;

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

int main(void)
{
    init_sensors();

    while (1)
    {
        read_temperature();
        read_pressure();
        read_light();
        ztimer_sleep(ZTIMER_MSEC, 5000);
    }

    return 0;
}
