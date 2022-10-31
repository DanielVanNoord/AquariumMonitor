#include "board.h"
#include "bme_sensor.h"
#ifndef SIMULATION_AM
#include <arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

Adafruit_BME280 bme; // I2C

#endif

bool bme280Present = false;

void init_bme_sensor()
{
#ifndef SIMULATION_AM
    bme280Present = bme.begin(0x76);
#endif
}

double read_bme_temp()
{
#ifndef SIMULATION_AM
    return bme.readTemperature();
#else
    return 20.12;
#endif
}

double read_bme_humidity()
{
#ifndef SIMULATION_AM
    return bme.readHumidity();
#else
    return 42.42;
#endif
}

double read_bme_pressure()
{
#ifndef SIMULATION_AM
    return bme.readPressure() / 100.0F;
#else
    return 1000.2;
#endif
}