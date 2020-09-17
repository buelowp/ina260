#include <INA260.h>

INA260 ina260;

SerialLogHandler logHandler;

void setup() 
{
}

void loop() 
{
    double current = ina260.current();
    double voltage = ina260.voltage();
    double power = ina260.power();

    Log.info("Current: %f mA, Voltage: %f mV, Power: %f mW", current, voltage, power);

    delay(1000);
}