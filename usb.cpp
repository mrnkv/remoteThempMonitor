#include "usb.h"

USB Usb;
ACMAsyncOper AsyncOper;
ACM Acm(&Usb, &AsyncOper);

uint8_t ACMAsyncOper::OnInit(ACM *pacm){
    Serial.println("ACM Init...");
    uint8_t rcode;
    rcode = pacm->SetControlLineState(3);
    if(rcode){
        ErrorMessage<uint8_t>(PSTR("SetControlLineState"), rcode);
        return rcode;
    }
    LINE_CODING lc;
    lc.dwDTERate    = 115200;
    lc.bCharFormat  = 0;
    lc.bParityType  = 0;
    lc.bDataBits    = 8;
    rcode = pacm->SetLineCoding(&lc);
    if(rcode){
        ErrorMessage<uint8_t>(PSTR("SetLineCoding"), rcode);
    }
    return rcode;
}

String sendCommand(ACM *Acm, String command){
    Usb.Task();
    uint8_t rcode;
    while(!Acm->isReady()){
        Serial.println("Acm waiting...");
        delay(100);
    };
    uint8_t cmd[]= {'A','T','+','C','M','G','L','\r'};
    rcode = Acm->SndData(8, cmd);
    if(rcode){
        ErrorMessage<uint8_t>(PSTR("SndData"), rcode);
    }
    delay(500);
    uint16_t rcvd = 64;
    uint8_t res[64];
    while(rcvd != 0){
        Usb.Task();
        while(!Acm->isReady()){};
        if(Acm->isReady()){
        rcode = Acm->RcvData(&rcvd, res);
        if(rcode && rcode != hrNAK){
            ErrorMessage<uint8_t>(PSTR("Ret"), rcode);
        }
        for(uint8_t i = 0; i<rcvd; ++i){
            Serial.print((char)res[i]);
        }
        }
        delay(10);
    }
    Serial.println("++++++++++++++++++++++++++++++++++++");
    return ""; 
}
