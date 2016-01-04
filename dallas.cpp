#include "dallas.h"
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress thermometer[THERMOMETERS];

void setupDallas(){
  Serial.println("Dallas Temperature IC Control");
  // Start up the library
  sensors.begin();
  // locate devices on the bus
  Serial.print("Locating devices...");
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");
  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  for(uint8_t i = 0; i < THERMOMETERS; ++i){
      if(!sensors.getAddress(thermometer[i], i)){
          Serial.print("Unable to find address for Device ");
          Serial.println(i, DEC);
      }
  }
  // show the addresses we found on the bus
  for(uint8_t i = 0; i < THERMOMETERS; ++i){
      Serial.print("Device ");
      Serial.print(i, DEC);
      Serial.print(" Address: ");
      printAddress(thermometer[i]);
      Serial.println();
  }
  for(uint8_t i = 0; i < THERMOMETERS; ++i){
      sensors.setResolution(thermometer[i], TEMPERATURE_PRECISION);
      Serial.print("Device ");
      Serial.print(i, DEC);
      Serial.print(" Resolution: ");
      Serial.print(sensors.getResolution(thermometer[i]), DEC); 
      Serial.println();
  }
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.print(tempC);
  Serial.print(" Temp F: ");
  Serial.print(DallasTemperature::toFahrenheit(tempC));
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();    
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}

void loop(void)
{ 
 //printData(insideThermometer);
  //printData(outsideThermometer);
}

