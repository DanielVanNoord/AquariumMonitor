#include "board.h"
#include "hall_sensors.h"
#ifndef SIMULATION_AM
#include <arduino.h>
#endif

void init_hall_sensors()
{
#ifndef SIMULATION_AM
    pinMode(HALL_1_PIN, INPUT_PULLUP);
    pinMode(HALL_2_PIN, INPUT_PULLUP);
#endif
}

int read_hall_sensor_1()
{
#ifndef SIMULATION_AM
    return digitalRead(HALL_1_PIN);
#else
    return 1;
#endif
}
int read_hall_sensor_2()
{
#ifndef SIMULATION_AM
    return digitalRead(HALL_2_PIN);
#else
    return 0;
#endif
}
