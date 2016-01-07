#include <cdcacm.h>
#include <usbhub.h>
#include <QueueList.h>
#include <Regexp.h>
#include <LiquidCrystal.h>

#include "pgmstrings.h"
#include "dallas.h"

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

class ACMAsyncOper : public CDCAsyncOper
{
public:
    uint8_t OnInit(ACM *pacm);
};

uint8_t ACMAsyncOper::OnInit(ACM *pacm)
{
    uint8_t rcode;
    // Set DTR = 1 RTS=1
    rcode = pacm->SetControlLineState(3);

    if (rcode)
    {
        ErrorMessage<uint8_t>(PSTR("SetControlLineState"), rcode);
        return rcode;
    }

    LINE_CODING	lc;
    lc.dwDTERate	= 115200;
    lc.bCharFormat	= 0;
    lc.bParityType	= 0;
    lc.bDataBits	= 8;

    rcode = pacm->SetLineCoding(&lc);

    if (rcode)
        ErrorMessage<uint8_t>(PSTR("SetLineCoding"), rcode);

    return rcode;
}

USB     Usb;
//USBHub     Hub(&Usb);
ACMAsyncOper  AsyncOper;
ACM           Acm(&Usb, &AsyncOper);

LiquidCrystal lcd(3, 4, 5, 6, 7, 8);

extern OneWire oneWire;
extern DallasTemperature sensors;
extern DeviceAddress thermometer[THERMOMETERS];
String objects[THERMOMETERS] = {"Kitchn", 
                                "Boiler", 
                                "  Room"};
String objectThemp[THERMOMETERS];

struct usbString{
    char str[128];
    uint16_t counter;
};

usbString s_in, s_out;
bool commandComplete;
QueueList<String> commandQueue;
String cmds[] = {"AT+CCLK?\r", "AT+CMGL=\"ALL\"\r"};
String smsText;

void selectCommand(){
    static uint16_t commandCounter = 0;
    if(commandQueue.isEmpty()){
        cmds[commandCounter].toCharArray(s_in.str, cmds[commandCounter].length()+1);
        s_in.counter = 0;
        s_out.counter = 0;
 //       Serial.println("Command " + cmds[commandCounter] + " selected");
        commandCounter++;
        if (commandCounter == 2) commandCounter = 0;
    }
    else{
        String s = commandQueue.pop();
 //       Serial.print("from QUEUE");
 //       Serial.println(s);
        s.toCharArray(s_in.str, s.length()+1);
        s_in.counter = 0;
        s_out.counter = 0;
    }
    delay(100);
}

void handleString(String s){
    MatchState ms;
    char buf[64];
    ms.Target((char*)s.c_str());
    //is it time answer?
    char result = ms.Match("%d+/%d+/%d+,%d+:%d+:%d+", 0);   
    if(result == REGEXP_MATCHED){
    //    Serial.println("Time Matched");

        String time = ms.GetMatch(buf);
    //    Serial.println(time);
        lcd.setCursor(0,0);
        lcd.print(time);
    }
    else{
    }
    result = ms.Match("+CMGL: (%d+), \"[%a%s]+\", \"(+%d+)\"", 0);
    if(result == REGEXP_MATCHED){
        //Serial.println("SMS Matched");
        //Serial.print("SMS num = ");
        String smsNum = ms.GetCapture(buf, 0);
        //Serial.println(smsNum);
        //Serial.print("Phone num = ");
        String phoneNum = ms.GetCapture(buf, 1);
        //Serial.println(phoneNum);
        //delete sms from phone
        //Send sms command series
        commandQueue.push("AT+CMGF=1\r");
        String smsString = "AT+CMGW=\"" + phoneNum+"\"\r" + "Hello from dacha ";
        smsString.setCharAt(smsString.length()-1, 26);
        commandQueue.push(smsString);
        commandQueue.push("AT+CMGD="+smsNum+"\r");
    }
    else{
    }
    result = ms.Match("+CMGW: (%d+)", 0);
    if(result == REGEXP_MATCHED){
        //Serial.println("SEND SMS Matched");
        String sendSMSNum = ms.GetCapture(buf, 0);
        commandQueue.push("AT+CMSS="+sendSMSNum+"\r");
        commandQueue.push("AT+CMGD="+sendSMSNum+"\r");
    }
                                             
}




void setup()
{
    Serial.begin( 115200 );
    setupDallas();
    lcd.begin(16, 2);
    commandComplete = true;
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
  Serial.println("Start");

  if (Usb.Init() == -1)
      Serial.println("OSCOKIRQ failed to assert");

  delay( 200 );
}

void loop()
{
    static uint8_t thempNumber = 0;
    static uint16_t loopCounter = 0;
    if (commandComplete){
        //Serial.println("Selecting command ...");
        selectCommand();
        commandComplete = false;
    }

      

    Usb.Task();

    if( Acm.isReady()) {
       uint8_t rcode;
       /* reading the keyboard */
       if(s_in.str[s_in.counter] != 0) {
         uint8_t data= s_in.str[s_in.counter];
         /* sending to the phone */
         rcode = Acm.SndData(1, &data);
         if (rcode)
            ErrorMessage<uint8_t>(PSTR("SndData"), rcode);
         s_in.counter++;
       }//if(Serial.available()...

       delay(50);

        /* reading the phone */
        /* buffer size must be greater or equal to max.packet size */
        /* it it set to 64 (largest possible max.packet size) here, can be tuned down
        for particular endpoint */
        uint8_t  buf[64];
        uint16_t rcvd = 64;
        rcode = Acm.RcvData(&rcvd, buf);
         if (rcode && rcode != hrNAK)
            ErrorMessage<uint8_t>(PSTR("Ret"), rcode);
            if( rcvd ) { //more than zero bytes received
              for(uint16_t i=0; i < rcvd; i++ ) {
                //Serial.print((char)buf[i]); //printing on the screen
                s_out.str[s_out.counter] = (char)buf[i];
                s_out.counter++;
                if(buf[i] == '\n' || buf[i] == '\r'){
                    s_out.str[s_out.counter] = 0;
                    String result(s_out.str);
                    for(uint8_t i = 0; i < result.length(); ++i){
                        if(result.charAt(i) == '\r' || result.charAt(i) == '\n'){
                            result.setCharAt(i, 32);
                        }
                    }
                    result.trim();
                    Serial.println(result);
                    if(result == "OK") {
                        commandComplete = true;
     //                   Serial.println("command complete");
                    }
                    else{
                        handleString(result);
                    }
                    s_out.counter = 0;
                }
              }
            }
        delay(10);
    }//if( Usb.getUsbTaskState() == USB_STATE_RUNNING..    
    
    //request themperatures
    /*
    if(loopCounter == 100){
        loopCounter = 0;
        sensors.requestTemperatures();
        //Serial.print("THEM = "); Serial.println(thempNumber, DEC);
        float themp = sensors.getTempC(thermometer[thempNumber]);
        objectThemp[thempNumber] = String(themp);
        String res = objects[thempNumber]+" "+String(themp) + " C";
        lcd.setCursor(0,1);
        lcd.print(res);
        thempNumber++;
        if(thempNumber == THERMOMETERS) thempNumber = 0;
        smsText = ""; 
        for(uint8_t i = 0; i < THERMOMETERS; ++i){
            smsText += objects[i] +":"+objectThemp[i]+"; ";
        }
        //Serial.println(smsText);
        delay(1000);
    }

    loopCounter++;
    */
}


