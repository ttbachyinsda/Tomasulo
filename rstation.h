#ifndef RSTATION_H
#define RSTATION_H
#include <QString>

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
};

#endif // RSTATION_H
