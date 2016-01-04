#include "usb.h"

USB Usb;
ACMAsyncOper AsyncOper;
ACM Acm(&Usb, &AsyncOper);

uint8_t ACMAsyncOper::OnInit(ACM *pacm){
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


