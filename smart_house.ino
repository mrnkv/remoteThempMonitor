#include <LiquidCrystal.h>
#include <Regexp.h>

#include "usb.h"
#include "dallas.h"

#define DEBUG

LiquidCrystal lcd(3, 4, 5, 6, 7, 8);

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
extern OneWire oneWire;

// Pass our oneWire reference to Dallas Temperature. 
extern DallasTemperature sensors;

// arrays to hold device addresses
extern DeviceAddress thermometer[THERMOMETERS];

extern USB Usb;
extern ACMAsyncOper AsyncOper;
extern ACM Acm;

String objects[THERMOMETERS] = {"Kitchn", 
                                "Boiler", 
                                "  Room"};


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
#ifdef DEBUG
//  Serial.print("Requesting temperatures...");
#endif
//  sensors.requestTemperatures();
#ifdef DEBUG
//  Serial.println("DONE");
#endif
  // print the device information
 // for(uint8_t i = 0; i < THERMOMETERS; ++i){
   //   printData(thermometer[i]);
  //}
    /* ===================================================== 
     *               USB routines                           *
     * =====================================================*/                           
    Usb.Task();
    if(Acm.isReady()){
        /* ============== check time =====================*/
        String t = sendCommand(&Acm, "AT+CCLK?\r");
        //Serial.println(t);
        /*
        MatchState ms;
        char buf[128];
        ms.Target((char*)t.c_str());
        char result = ms.Match("%d+/%d+/%d+,%d+:%d+:%d+", 0);
        String time = ms.GetMatch(buf);
       if(result == REGEXP_MATCHED){
          Serial.println(time);
          lcd.setCursor(0,0);
          lcd.print(time);
       }
       else{
          Serial.println("Timer Not matched"); 
       }
       */
       /* =============== check unread messages ==========*/
       String m = sendCommand(&Acm, "AT+CMGL=\"ALL\"\r");
       //delay(100);
       //Serial.println(m);
      /* 
        ms.Target((char*)m.c_str());
        result = ms.Match("+CMGL (%d+), \"[%a+%s+]\", \"(+7d%)\"", 0);
        if(result == REGEXP_MATCHED){
            Serial.print("level = "); Serial.println(ms.level, DEC);
            for(uint8_t i = 0; i < ms.level; ++i){
                Serial.println(ms.GetCapture(buf, i));
            }

        }
        else{
            Serial.println("SMS pattern not matched");
        }
        //+CMGL: 1, "REC READ", "+79202474611"
      */
    }
     /*==================== End USB routines =========================*/
    /*
    for(uint8_t i = 0; i < THERMOMETERS; ++i){
        float themp = sensors.getTempC(thermometer[i]);
        String res = objects[i]+" "+String(themp) + " C";
        lcd.setCursor(0,1);
        lcd.print("            ");
        lcd.setCursor(0,1);
        lcd.print(res);
        delay(1000);
    }
    */
}
