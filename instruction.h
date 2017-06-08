#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include <QString>
#include "rstation.h"

class Instruction
{
public:
    Instruction();
    QString Op, F1, F2, F3;
    int trueop,truef1,truef2,truef3,truer;
};

#endif // INSTRUCTION_H
