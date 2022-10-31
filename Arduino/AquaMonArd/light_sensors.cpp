#include "board.h"
#include "light_sensors.h"
#ifndef SIMULATION_AM
#include <arduino.h>
#endif

void init_light_sensors()
{
#ifndef SIMULATION_AM
#endif
}

int read_light_sensor_1()
{
#ifndef SIMULATION_AM
    return analogRead(A0);
#else
    return 101;
#endif
}
int read_light_sensor_2()
{
#ifndef SIMULATION_AM
    return analogRead(A1);
#else
    return 102;
#endif
}
