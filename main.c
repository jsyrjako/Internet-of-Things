#include <stdio.h>
#include <stdlib.h>
#include <lpsxxx.h>
#include <isl29020.h>

#define ISL29020_ADDR 0x44
#define LPS331AP_ADDR 0x5c

struct lps331ap_params_t
{
    i2c_t i2c = LPSXXX_PARAM_I2C; // I2C bus the sensor is connected to
    uint8_t addr = LPS331AP_ADDR; // the device's address on the bus
    lpsxxx_rate_t rate = LPSXXX_PARAM_RATE ; // tell sensor to sample with this rate
};

int init_sensors(void)
{

    struct lps331ap_params_t sensor_params;

    if (lps331ap_init(&sensor_params) != 0) {
        perror("Failed to initialize sensor LPS331AP");
    }


}

int main(void)
{


    return 0;
}















void read_sensor_data(int fd, int addr, int reg)
// {
//     // Set I2C slave address
//     if (ioctl(fd, I2C_SLAVE, addr) < 0) {
//         perror("Failed to set I2C slave address");
//         exit(1);
//     }

//     // Read data from sensor
//     buf[0] = reg;
//     if (write(fd, buf, 1) != 1) {
//         perror("Failed to write to I2C bus");
//         exit(1);
//     }

//     if (read(fd, buf, 2) != 2) {
//         perror("Failed to read from I2C bus");
//         exit(1);
//     }

//     int data = (buf[0] << 8) | buf[1];
//     printf("Data: %d\n", data);
// }
