#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9
#define THERMOMETERS 3

extern OneWire oneWire;
extern DallasTemperature sensors;
extern DeviceAddress thermometer[THERMOMETERS];

void printAddress(DeviceAddress deviceAddress);
void printTemperature(DeviceAddress deviceAddress);
void printResolution(DeviceAddress deviceAddress);
void printData(DeviceAddress deviceAddress);
void setupDallas();

