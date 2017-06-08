#include "status.h"

Status::Status()
{
    Opconvert[OPADD] = "ADDD";
    Opconvert[OPMINUS] = "SUBD";
    Opconvert[OPMULTIPLY] = "MULD";
    Opconvert[OPDIVIDE] = "DIVD";
    Opconvert[OPLOAD] = "LD";
    Opconvert[OPSTORE] = "ST";
    for (int i=0;i<bufferlen;i++){
        RStation loadstore;
        loadstore.truename = i + LENADDRESERVATION + LENMULTIPLYRESERVATION;
        Buffer.append(loadstore);
    }
    for (int i=0;i<registerlen;i++){
        Register reg;
        reg.Name="R"+QString::number(i);
        reg.truename = i;
        reg.V = i+1;
        IntRegister.append(reg);
    }
    for (int i=0;i<registerlen;i++){
        FRegister reg;
        reg.Name="F"+QString::number(i);
        reg.truename = i + LENREGISTER;
        reg.V = i+1;
        FloatRegister.append(reg);
    }
    for (int i=0;i<addreservation;i++){
        RStation res;
        res.truename = i;
        res.Name = "ADD"+QString::number(i+1);
        AddReservation.append(res);
    }
    for (int i=0;i<multireservation;i++){
        RStation res;
        res.truename = i+LENADDRESERVATION;
        res.Name = "MULT"+QString::number(i+1);
        MultiplyReservation.append(res);
    }
    for (int i=0;i<memorylen;i++){
        Memory.append(0);
    }
    BufferPointer = NULL;
    instructPointer = 0;
    AddReservationPointer = NULL;
    MultiplyReservationPointer = NULL;
    CurrentInstruction = 0;
    Isdone = false;
    NowCycle = 0;
    CDBresultData = 0;
    CDBresultEnable = false;
    CDBresultReservation = -1;

    for (int i=0;i<LENBUFFER;i++){
        RStation bn;
        bn.truename = i+LENADDRESERVATION + LENMULTIPLYRESERVATION;
        bufferNext.append(bn);
    }
    for (int i=0;i<LENADDRESERVATION;i++){
        RStation arn;
        arn.truename = i;
        addReservationNext.append(arn);
    }
    for (int i=0;i<LENMULTIPLYRESERVATION;i++){
        RStation mrn;
        mrn.truename = i+LENADDRESERVATION;
        multiplyReservationNext.append(mrn);
    }
    for (int i=0;i<LENREGISTER;i++){
        Register rn;
        rn.truename = i;
        rn.V = i+1;
        registerNext.append(rn);
    }
    for (int i=0;i<LENREGISTER;i++){
        FRegister frn;
        frn.truename = i + LENREGISTER;
        frn.V = i+1;
        floatRegisterNext.append(frn);
    }
    for (int i=0;i<LENMEMORY;i++){
        float mn = 0;
        memoryNext.append(mn);
    }
    CDBresultDataNext = 0;
    CDBresultEnableNext = false;
    CDBresultReservationNext = -1;
    storeData = 0.0;
    storeEnable = false;
    storeLoc = 0;
}

//IMPORTANT
//需要实现restart，即status的重新开始功能
void Status::restart()
{

}
void Status::LOCKER(bool lockstatus)
{
    Lock = lockstatus;
}

//IMPORTANT
//需要实现nextstep，即status的下一步功能
int Status::nextstep(){
    CDBresultEnableNext = false;
    int errorcode = ERRRIGHT;
    errorcode = updateOut();
    if (errorcode != 0) ERROR_RETURN();
    errorcode = updateAddReservation();
    if (errorcode != 0) ERROR_RETURN();
    errorcode = updateMultiplyReservation();
    if (errorcode != 0) ERROR_RETURN();
    errorcode = updateBuffer();
    if (errorcode != 0) ERROR_RETURN();
    errorcode = updateRegister();
    if (errorcode != 0) ERROR_RETURN();
    errorcode = updateFinal();
    ERROR_RETURN();
}

FRegister* Status::getFRegister(int name){
    FRegister *f = NULL;
    for (int i=0;i<LENREGISTER;i++){
        if (FloatRegister[i].truename == name){
            f = &FloatRegister[i];
            break;
        }
    }
    return f;
}

FRegister* Status::getFRegisterNext(int name){
    FRegister *f = NULL;
    for (int i=0;i<LENREGISTER;i++){
        if (floatRegisterNext[i].truename == name){
            f = &floatRegisterNext[i];
            break;
        }
    }
    return f;
}

Register* Status::getRegister(int name)
{
    Register *f = NULL;
    for (int i=0;i<LENREGISTER;i++){
        if (IntRegister[i].truename == name){
            f = &IntRegister[i];
            break;
        }
    }
    return f;
}

int Status::updateOut(){
    FRenable = false;
    int errorcode = ERRRIGHT;
    if (instructPointer >= InstructList.size()) return errorcode;
    Instruction* inst = &InstructList[instructPointer];
    switch (inst->trueop) {
    case OPADD:
    case OPMINUS:
    {
        for (int i=0;i<LENADDRESERVATION; i++){
            if (!AddReservation[i].IsBusy){
                addReservationNext[i].IsBusy = true;
                addReservationNext[i].trueop = inst->trueop;
                addReservationNext[i].Time = -1;
                FRegister *F2 = getFRegister(inst->truef2);
                if (F2->Q == -1){
                    addReservationNext[i].QJ = -1;
                    addReservationNext[i].VJ = F2->V;
                } else
                    addReservationNext[i].QJ = F2->Q;
                FRegister *F3 = getFRegister(inst->truef3);
                if (F3->Q == -1){
                    addReservationNext[i].QK = -1;
                    addReservationNext[i].VK = F3->V;
                } else
                    addReservationNext[i].QK = F3->Q;
                addReservationNext[i].Time = -1;
                FRenable = true;
                FRres = addReservationNext[i].truename;
                FRname = inst->truef1;
                ++instructPointer;
                break;
            }
        }
        break;
    }
    case OPMULTIPLY:
    case OPDIVIDE:
    {
        for (int i=0;i<LENMULTIPLYRESERVATION;i++){
            if (!MultiplyReservation[i].IsBusy){
                multiplyReservationNext[i].IsBusy = true;
                multiplyReservationNext[i].trueop = inst->trueop;
                multiplyReservationNext[i].Time = -1;
                FRegister *F2 = getFRegister(inst->truef2);
                if (F2->Q == -1){
                    multiplyReservationNext[i].QJ = -1;
                    multiplyReservationNext[i].VJ = F2->V;
                }
                else
                    multiplyReservationNext[i].QJ = F2->Q;
                FRegister *F3 = getFRegister(inst->truef3);
                if (F3->Q == -1){
                    multiplyReservationNext[i].QK = -1;
                    multiplyReservationNext[i].VK = F2->V;
                }
                else
                    multiplyReservationNext[i].QK = F2->Q;
                multiplyReservationNext[i].Time = -1;
                FRenable = true;
                FRres = multiplyReservationNext[i].truename;
                FRname = inst->truef1;
                instructPointer++;
                break;
            }
        }
        break;
    }
    case OPLOAD:
    {
        for (int i=0;i<LENBUFFER;i++){
            if (!Buffer[i].IsBusy){
                bufferNext[i].IsBusy = true;
                bufferNext[i].trueop = inst->trueop;
                bufferNext[i].Time = -1;
//                Register *F2 = getRegister(inst->truef2);
//                if (F2->Q == -1) {
//                    bufferNext[i].QJ = -1;
//                    bufferNext[i].VJforBuffer = F2->V;
//                }
//                else
//                    bufferNext[i].QJ = F2->Q;
                bufferNext[i].A = inst->truef2;
                bufferNext[i].QK = -1;
                bufferNext[i].QJ = -1;
                FRenable = true;
                FRres = bufferNext[i].truename;
                FRname = inst->truef1;
                instructPointer++;
                break;
            }
        }
        break;
    }
    case OPSTORE:
    {
        for (int i=0;i<LENBUFFER;i++){
            if (!bufferNext[i].IsBusy){
                bufferNext[i].IsBusy = true;
                bufferNext[i].trueop = inst->trueop;
                bufferNext[i].Time = -1;
//                Register* F2 = getRegister(inst->truef2);
//                if (F2->Q == -1){
//                    bufferNext[i].QJ = -1;
//                    bufferNext[i].VJforBuffer = F2->V;
//                }
//                else
//                    bufferNext[i].QJ = F2->Q;
                bufferNext[i].A = inst->truef2;
                bufferNext[i].Time = -1;
                bufferNext[i].QJ = -1;
                FRegister *F1 = getFRegister(inst->truef1);
                if (F1->Q == -1){
                    bufferNext[i].QK = -1;
                    bufferNext[i].VK = F1->V;
                }
                else
                    bufferNext[i].QK = F1->Q;
                instructPointer++;
                break;
            }
        }
        break;
    }
    default:
        qDebug()<<"ERROR AT THIS";
    }
    return errorcode;
}
int Status::updateAddReservation() {
    int errorcode = ERRRIGHT;
    if (CDBresultEnable) {
        for (int i=0;i<LENADDRESERVATION;i++){
            if (AddReservation[i].truename == CDBresultReservation){
                addReservationNext[i].IsBusy = false;
            }
            if (AddReservation[i].QJ == CDBresultReservation){
                addReservationNext[i].QJ = -1;
                addReservationNext[i].VJ = CDBresultData;
            }
            if (AddReservation[i].QK == CDBresultReservation){
                addReservationNext[i].QK = -1;
                addReservationNext[i].VK = CDBresultData;
            }
        }
    }
    if (AddReservationPointer == NULL){
        for (int i=0;i<LENADDRESERVATION;i++){
            if (addReservationNext[i].IsBusy) {
                if (addReservationNext[i].QK == -1 && addReservationNext[i].QJ == -1){
                    AddReservationPointer = &addReservationNext[i];
                    addReservationNext[i].Time = CYCLEADD;
                    break;
                }
            }
        }
    } else {
        if (AddReservationPointer->Time > 0)
            --AddReservationPointer->Time;
        if (AddReservationPointer->Time == 0){
            if (CDBresultEnableNext == false) {
                CDBresultEnableNext = true;
                CDBresultReservationNext = AddReservationPointer->truename;
                switch (AddReservationPointer->trueop) {
                case OPADD:
                {
                    CDBresultDataNext = AddReservationPointer->VJ + AddReservationPointer->VK;
                    break;
                }
                case OPMINUS:
                {
                    CDBresultDataNext = AddReservationPointer->VJ - AddReservationPointer->VK;
                    break;
                }
                default:
                    qDebug()<<"ERROR AT THAT";
                }
                AddReservationPointer = NULL;
                for (int i=0;i<LENADDRESERVATION;i++){
                    if (AddReservation[i].IsBusy && AddReservation[i].truename != CDBresultReservationNext) {
                        if (AddReservation[i].QK == -1 && AddReservation[i].QJ == -1) {
                            AddReservationPointer = &addReservationNext[i];
                            addReservationNext[i].Time = CYCLEADD;
                            break;
                        }
                    }
                }
            }
        }
    }
    return errorcode;
}

int Status::updateMultiplyReservation() {
    int errorcode = ERRRIGHT;
    if (CDBresultEnable){
        for (int i=0;i<LENMULTIPLYRESERVATION;i++){
            if (MultiplyReservation[i].truename == CDBresultReservation){
                multiplyReservationNext[i].IsBusy = false;
            }
            if (MultiplyReservation[i].QJ == CDBresultReservation){
                multiplyReservationNext[i].QJ = -1;
                multiplyReservationNext[i].VJ = CDBresultData;
            }
            if (MultiplyReservation[i].QK == CDBresultReservation){
                multiplyReservationNext[i].QK = -1;
                multiplyReservationNext[i].VK = CDBresultData;
            }
        }
    }
    if (MultiplyReservationPointer == NULL){
        for (int i=0;i<LENMULTIPLYRESERVATION;i++){
            if (multiplyReservationNext[i].IsBusy) {
                if (multiplyReservationNext[i].QK == -1 && multiplyReservationNext[i].QJ == -1){
                    MultiplyReservationPointer = &multiplyReservationNext[i];
                    switch(MultiplyReservationPointer->trueop){
                    case OPMULTIPLY:
                    {
                        multiplyReservationNext[i].Time = CYCLEMULTIPLY;
                        break;
                    }
                    case OPDIVIDE:
                    {
                        multiplyReservationNext[i].Time = CYCLEDIVIDE;
                        break;
                    }
                    default:
                        qDebug()<<"ERROR AT THISTHAT";
                    }
                    break;
                }
            }
        }
    } else {
        if (MultiplyReservationPointer->Time > 0) -- MultiplyReservationPointer->Time;
        if (MultiplyReservationPointer->Time == 0) {
            if (CDBresultEnableNext == false){
                CDBresultEnableNext = true;
                CDBresultReservationNext = MultiplyReservationPointer->truename;
                switch (MultiplyReservationPointer->trueop) {
                case OPMULTIPLY:
                {
                    CDBresultDataNext = MultiplyReservationPointer->VJ * MultiplyReservationPointer->VK;
                    break;
                }
                case OPDIVIDE:
                {
                    if (MultiplyReservationPointer->VK == 0) return ERRDIVZERO;
                    CDBresultDataNext = MultiplyReservationPointer->VJ / MultiplyReservationPointer->VK;
                    break;
                }
                default:
                    qDebug()<<"ERROR ATTHIS";
                }
                MultiplyReservationPointer = NULL;
                for (int i=0;i<LENMULTIPLYRESERVATION;i++){
                    if (MultiplyReservation[i].IsBusy && MultiplyReservation[i].truename != CDBresultReservationNext) {
                        if (MultiplyReservation[i].QK == -1 && MultiplyReservation[i].QJ == -1){
                            MultiplyReservationPointer = &multiplyReservationNext[i];
                            switch (MultiplyReservationPointer->trueop){
                            case OPMULTIPLY:
                            {
                                CDBresultDataNext = MultiplyReservationPointer->VJ * MultiplyReservationPointer->VK;
                                break;
                            }
                            case OPDIVIDE:
                            {
                                if (MultiplyReservationPointer->VK == 0) return ERRDIVZERO;
                                CDBresultDataNext = MultiplyReservationPointer->VJ / MultiplyReservationPointer->VK;
                                break;
                            }
                            default:
                                qDebug()<<"ERRORATTHIS";
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
    return errorcode;
}

int Status::updateBuffer() {
    int errorcode = ERRRIGHT;
    if (storeEnable) {
        memoryNext[storeLoc % LENMEMORY] = storeData;
    }
    storeEnable = false;
    if (CDBresultEnable) {
        for (int i=0;i<LENBUFFER;i++){
            if (Buffer[i].QJ == CDBresultReservation){
                qDebug()<<"该系统中这里只应存放准备好的整数寄存器的内容";
            }
            if (Buffer[i].QK == CDBresultReservation){
                Buffer[i].QK = -1;
                Buffer[i].VK = CDBresultData;
            }
        }
    }
    if (BufferPointer == NULL){
        if (Buffer[0].IsBusy && Buffer[0].QK == -1){
            BufferPointer = &bufferNext[0];
            BufferPointer->Time = CYCLELOAD;
        }
    }
    if (BufferPointer){
        bool flag = false;
        if (BufferPointer->Time > 0) --BufferPointer->Time;
        if (BufferPointer->Time == 0){
//            BufferPointer->A += BufferPointer->VJforBuffer;
            if (BufferPointer->trueop == OPSTORE) {
                storeEnable = true;
                storeData = BufferPointer->VK;
                storeLoc = BufferPointer->A;
                flag = true;
            }
            else {
                if (!CDBresultEnableNext){
                    CDBresultEnableNext = true;
                    CDBresultReservationNext = BufferPointer->truename;
                    CDBresultDataNext = Memory[BufferPointer->A % LENMEMORY];
                    flag = true;
                }
            }
        }
        if (flag) {
            int name = BufferPointer->truename;
            BufferPointer = NULL;
            bufferNext.erase(bufferNext.begin());
            RStation n;
            n.truename = name;
            n.Time = -1;
            bufferNext.append(n);
            if (Buffer[1].IsBusy && Buffer[1].QK == -1){
                BufferPointer = &bufferNext[0];
                BufferPointer->Time = CYCLELOAD;
            }
        }
    }
    return errorcode;
}

int Status::updateFinal() {
    int errorcode = ERRRIGHT;
    NowCycle++;
    if (instructPointer >= InstructList.size()){
        if (false) {
            return ERRALREADYEND;
        }
    }
    for (int i=0;i<LENADDRESERVATION;i++) {
        AddReservation[i].IsBusy = addReservationNext[i].IsBusy;
        AddReservation[i].trueop = addReservationNext[i].trueop;
        AddReservation[i].Time = addReservationNext[i].Time;
        AddReservation[i].QJ = addReservationNext[i].QJ;
        AddReservation[i].QK = addReservationNext[i].QK;
        AddReservation[i].VJ = addReservationNext[i].VJ;
        AddReservation[i].VK = addReservationNext[i].VK;
        AddReservation[i].truename = addReservationNext[i].truename;
    }
    for (int i=0;i<LENMULTIPLYRESERVATION;i++){
        MultiplyReservation[i].truename = multiplyReservationNext[i].truename;
        MultiplyReservation[i].IsBusy = multiplyReservationNext[i].IsBusy;
        MultiplyReservation[i].trueop = multiplyReservationNext[i].trueop;
        MultiplyReservation[i].Time = multiplyReservationNext[i].Time;
        MultiplyReservation[i].QJ = multiplyReservationNext[i].QJ;
        MultiplyReservation[i].QK = multiplyReservationNext[i].QK;
        MultiplyReservation[i].VJ = multiplyReservationNext[i].VJ;
        MultiplyReservation[i].VK = multiplyReservationNext[i].VK;
    }
    for (int i=0;i<LENBUFFER;i++){
        Buffer[i].truename = bufferNext[i].truename;
        Buffer[i].IsBusy = bufferNext[i].IsBusy;
        Buffer[i].trueop = bufferNext[i].trueop;
        Buffer[i].Time = bufferNext[i].Time;
        Buffer[i].QJ = bufferNext[i].QJ;
        Buffer[i].QK = bufferNext[i].QK;
        Buffer[i].VJ = bufferNext[i].VJ;
        Buffer[i].VK = bufferNext[i].VK;
        Buffer[i].A = bufferNext[i].A;
        Buffer[i].VJforBuffer = bufferNext[i].VJforBuffer;
    }
    for (int i=0;i<LENREGISTER;i++){
        IntRegister[i].truename = registerNext[i].truename;
        IntRegister[i].Q = registerNext[i].Q;
        IntRegister[i].V = registerNext[i].V;
        FloatRegister[i].truename = floatRegisterNext[i].truename;
        FloatRegister[i].Q = floatRegisterNext[i].Q;
        FloatRegister[i].V = floatRegisterNext[i].V;
    }
    for (int i=0;i<LENMEMORY;i++){
        Memory[i] = memoryNext[i];
    }
    CDBresultData = CDBresultDataNext;
    CDBresultEnable = CDBresultEnableNext;
    CDBresultReservation = CDBresultReservationNext;

    return errorcode;
}

int Status::updateRegister()
{
    int errorcode = ERRRIGHT;
    if (CDBresultEnable) {
        for (int i=0;i<LENREGISTER;i++){
            if (FloatRegister[i].Q == CDBresultReservation){
                floatRegisterNext[i].V = CDBresultData;
                floatRegisterNext[i].Q = -1;
            }
            if (IntRegister[i].Q == CDBresultReservation){
                registerNext[i].V = (int)CDBresultData;
                registerNext[i].Q = -1;
            }
        }
    }
    if (FRenable) {
        FRegister *f = getFRegisterNext(FRname);
        f->Q = FRres;
    }
    return errorcode;
}

int Status::updateName()
{
    qDebug()<<"updateName";
    for (int i=0;i<bufferlen;i++){
        Buffer[i].Name = QString::number(Buffer[i].truename);
        Buffer[i].Op = Opconvert[Buffer[i].trueop];
    }
    for (int i=0;i<registerlen;i++){
        IntRegister[i].Name = QString::number(IntRegister[i].truename);
    }
    for (int i=0;i<registerlen;i++){
        FloatRegister[i].Name = QString::number(FloatRegister[i].truename);
    }
    for (int i=0;i<addreservation;i++){
        AddReservation[i].Name = QString::number(AddReservation[i].truename);
        AddReservation[i].Op = Opconvert[AddReservation[i].trueop];
    }
    for (int i=0;i<multireservation;i++){
        MultiplyReservation[i].Name = QString::number(MultiplyReservation[i].truename);
        MultiplyReservation[i].Op = Opconvert[MultiplyReservation[i].trueop];
    }
    qDebug()<<"updateNameend";
}
