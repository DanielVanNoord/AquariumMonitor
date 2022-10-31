#include "board.h"
#include "water_temp_sensor.h"
#ifndef SIMULATION_AM
#include <arduino.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "one_wire.h"

One_wire one_wire(WATER_PROBE_1);
One_wire one_wire2(WATER_PROBE_2);

#endif

void init_water_temp_sensors()
{
#ifndef SIMULATION_AM
    one_wire.init();
    one_wire2.init();
#endif
}
double read_water_temp_sensor_1()
{
#ifndef SIMULATION_AM
    rom_address_t address{};

    one_wire.single_device_read_rom(address);
    one_wire.convert_temperature(address, true, false);

    return one_wire.temperature(address);
#else
    return 23.11;
#endif
}
double read_water_temp_sensor_2()
{
#ifndef SIMULATION_AM
    rom_address_t address2{};

    one_wire2.single_device_read_rom(address2);
    one_wire2.convert_temperature(address2, true, false);

    return one_wire2.temperature(address2);
#else
    return 23.22;
#endif
}
