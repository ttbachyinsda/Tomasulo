#ifndef FREGISTER_H
#define FREGISTER_H
#include <QString>
#include "rstation.h"

class FRegister
{
public:
    FRegister();
    QString Name;
    int truename;
    int Q;
    float V;
    QString QName;
};

#endif // FREGISTER_H
