#include <LiquidCrystal.h>
#include "usb.h"
#include "dallas.h"

LiquidCrystal lcd(3, 4, 5, 6, 7, 8);

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
extern OneWire oneWire;

// Pass our oneWire reference to Dallas Temperature. 
extern DallasTemperature sensors;

// arrays to hold device addresses
extern DeviceAddress thermometer;

extern USB Usb;
extern ACMAsyncOper AsyncOper;
extern ACM Acm;


void setup() {
    Serial.begin(115200);
    Serial.println("Start!");
    if (Usb.Init() == -1){
        Serial.println("OSCOKIRQ failed to assert");
    }
    delay(200);
    setupDallas();
    lcd.begin(16, 2);
}

// the loop function runs over and over again forever
void loop() {  
  // call sensors.requestTemperatures() to issue a global temperature 
  // request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures();
  Serial.println("DONE");

  // print the device information
  for(uint8_t i = 0; i < THERMOMETERS; ++i){
      printData(thermometer[i]);
  }
 
    Usb.Task();
    if(Acm.isReady()){
        uint8_t rcode;
        /*reading keyboard*/
        if(Serial.available()){
            uint8_t data = Serial.read();
            /*sending to cell phone*/
            rcode = Acm.SndData(1, &data);
            if(rcode){
                ErrorMessage<uint8_t>(PSTR("SndData"), rcode);
            }
        }
    

        delay(50);

        uint8_t buf[64];
        uint16_t rcvd = 64;
        rcode = Acm.RcvData(&rcvd, buf);
        if(rcode && rcode != hrNAK){
            ErrorMessage<uint8_t>(PSTR("Ret"), rcode);
        }
        if(rcvd){
            for(uint16_t i = 0; i < rcvd; i++){
                Serial.print((char)buf[i]);
            }
        }
        delay(10);
    }
}
