#include "board.h"
#include "leak_sensor.h"
#ifndef SIMULATION_AM
#include <arduino.h>
#endif

void init_leak_sensor()
{
#ifndef SIMULATION_AM
#endif
}
int read_leak_sensor()
{
#ifndef SIMULATION_AM
        return analogRead(A2);
#else
        return 0;
#endif
}