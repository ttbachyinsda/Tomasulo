#ifndef RSTATION_H
#define RSTATION_H
#include <QString>
#include "instruction.h"
class Instruction;
class RStation
{
public:
    RStation();
    int Time;
    QString Name;
    int truename,trueop;
    bool IsBusy;
    QString Op;
    float VJ;
    float VK;
    int VJforBuffer;
    int QJ;
    int QK;
    int A;
    Instruction* inst;
};

#endif // RSTATION_H
