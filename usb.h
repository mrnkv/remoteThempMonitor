#include <cdcacm.h>
#include <usbhub.h>

#include "pgmstrings.h"

const int CTRL_Z = 26;

class ACMAsyncOper: public CDCAsyncOper{
    public:
    uint8_t OnInit(ACM *pacm);
};

String sendCommand(ACM *Acm, String command);
