#include "rstation.h"

RStation::RStation()
{
    Time = -1;
    Name = "";
    this->truename = -1;
    IsBusy = false;
    Op = "";
    this->trueop = 0;
    this->VJforBuffer = 0;
    VJ = 0.0;
    VK = 0.0;
    QJ = -1;
    QK = -1;
    A = 0;
}
