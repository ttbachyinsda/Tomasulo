#ifndef REGISTER_H
#define REGISTER_H
#include <QString>
#include "rstation.h"
class Register
{
public:
    Register();
    QString Name;
    int truename;
    int Q;
    QString QName;
    int V;
};

#endif // REGISTER_H
