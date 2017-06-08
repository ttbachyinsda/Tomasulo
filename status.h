#ifndef STATUS_H
#define STATUS_H
#include "rstation.h"
#include "fregister.h"
#include "register.h"
#include "instruction.h"
#include <QList>
#include <QDebug>
#include <QMap>
#define ERROR_RETURN() {updateName(); return errorcode;}

#define bufferlen 6
#define addreservation 3
#define LENADDRESERVATION 3
#define multireservation 2
#define LENMULTIPLYRESERVATION 2
#define memorylen 4096
#define LENMEMORY 4096
#define LENBUFFER 6
#define registerlen 16
#define LENREGISTER 16
#define ERRRIGHT 0
#define ERRDIVZERO -1
#define ERRALREADYSTART -2
#define ERRALREADYEND -3
#define ERRNOINSTRUCT -4
#define OPADD 101
#define OPMINUS 102
#define OPMULTIPLY 201
#define OPDIVIDE 202
#define OPLOAD 301
#define OPSTORE 302
#define CYCLEADD 2
#define CYCLELOAD 2
#define CYCLEMULTIPLY 10
#define CYCLEDIVIDE 40

#define ILLEGALINSTRUCT 101
#define DIVZERO 102
#define RIGHT 0
class Status
{
public:
    Status();
    void init();
    QList<RStation> Buffer, AddReservation, MultiplyReservation;
    QList<Instruction> InstructList;
    RStation *BufferPointer, *AddReservationPointer, *MultiplyReservationPointer;
    int instructPointer;
    QList<Register> IntRegister;
    QList<FRegister> FloatRegister;
    QList<float> Memory;
    QList<float> preMemory;
    float CDBresultData;
    int CDBresultReservation;
    bool CDBresultEnable;
    QList<RStation> addReservationNext, multiplyReservationNext;
    QList<RStation*> bufferNext;
    QList<Register> registerNext;
    QList<FRegister> floatRegisterNext;
    QList<float> memoryNext;
    float CDBresultDataNext;
    int CDBresultReservationNext;
    bool CDBresultEnableNext;
    int FRname;
    int FRres;
    bool FRenable;
    int storeLoc;
    float storeData;
    bool storeEnable;
    bool flag;

    int CurrentInstruction, NowCycle;
    bool Isdone, Lock;
    int nextstep();
    void restart();
    void LOCKER(bool lockstatus);
    int updateOut();
    int updateAddReservation();
    int updateMultiplyReservation();
    int updateBuffer();
    int updateFinal();
    int updateRegister();
    int updateName();
    FRegister* getFRegister(int name);
    FRegister* getFRegisterNext(int name);
    Register* getRegister(int name);
    QMap<int,QString> Opconvert;
    QMap<int,QString> Loadstoreconvert;
    QMap<int,QString> Nameconvert;
    QMap<QString,int> registerconvert;
};

#endif // STATUS_H
