#include "instruction.h"

Instruction::Instruction()
{
    Op = "";
    F1 = "";
    F2 = "";
    F3 = "";
    this->truef1 = -1;
    this->truef2 = -1;
    this->truef3 = -1;
    this->trueop = -1;
    this->truer = -1;
}
