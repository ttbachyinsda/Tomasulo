#include "status.h"

Status::Status()
{
    init();
}

void Status::init()
{
    for (int i=0;i<InstructList.size();i++)
    {
        InstructList[i].startTime = -1;
        InstructList[i].completeTime = -1;
        InstructList[i].wbTime = -1;
    }
    for (int i=0;i<LENREGISTER;i++)
    {
        registerconvert["R"+QString::number(i)] = i;
        registerconvert["F"+QString::number(i)] = i+LENREGISTER;
    }
    for (int i=0;i<LENADDRESERVATION;i++)
    {
        Nameconvert[i] = "ADD"+QString::number(i+1);
    }
    for (int i=0;i<LENMULTIPLYRESERVATION;i++)
    {
        Nameconvert[i+LENADDRESERVATION] = "MULT"+QString::number(i+1);
    }
    registerconvert["ADDD"] = OPADD;
    registerconvert["MULD"] = OPMULTIPLY;
    registerconvert["SUBD"] = OPMINUS;
    registerconvert["DIVD"] = OPDIVIDE;
    registerconvert["LD"] = OPLOAD;
    registerconvert["ST"] = OPSTORE;
    Opconvert[OPADD] = "ADDD";
    Opconvert[OPMINUS] = "SUBD";
    Opconvert[OPMULTIPLY] = "MULD";
    Opconvert[OPDIVIDE] = "DIVD";
    Opconvert[OPLOAD] = "LD";
    Opconvert[OPSTORE] = "ST";
    Buffer.clear();
    for (int i=0;i<bufferlen;i++){
        RStation loadstore;
        loadstore.truename = i + LENADDRESERVATION + LENMULTIPLYRESERVATION;
        Loadstoreconvert[i+LENADDRESERVATION+LENMULTIPLYRESERVATION] = "Buffer"+QString::number(i+1);
        Buffer.append(loadstore);
    }
    IntRegister.clear();
    for (int i=0;i<registerlen;i++){
        Register reg;
        reg.Name="R"+QString::number(i);
        reg.truename = i;
        reg.V = i+1;
        IntRegister.append(reg);
    }
    FloatRegister.clear();
    for (int i=0;i<registerlen;i++){
        FRegister reg;
        reg.Name="F"+QString::number(i);
        reg.truename = i + LENREGISTER;
        reg.V = i+1;
        FloatRegister.append(reg);
    }
    AddReservation.clear();
    for (int i=0;i<addreservation;i++){
        RStation res;
        res.truename = i;
        res.Name = "ADD"+QString::number(i+1);
        AddReservation.append(res);
    }
    MultiplyReservation.clear();
    for (int i=0;i<multireservation;i++){
        RStation res;
        res.truename = i+LENADDRESERVATION;
        res.Name = "MULT"+QString::number(i+1);
        MultiplyReservation.append(res);
    }
    Memory.clear();
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
    flag  = false;

    bufferNext.clear();
    for (int i=0;i<LENBUFFER;i++){
        RStation *bn = new RStation();
        bn->truename = i+LENADDRESERVATION + LENMULTIPLYRESERVATION;
        bufferNext.append(bn);
    }

    addReservationNext.clear();
    for (int i=0;i<LENADDRESERVATION;i++){
        RStation arn;
        arn.truename = i;
        addReservationNext.append(arn);
    }
    multiplyReservationNext.clear();
    for (int i=0;i<LENMULTIPLYRESERVATION;i++){
        RStation mrn;
        mrn.truename = i+LENADDRESERVATION;
        multiplyReservationNext.append(mrn);
    }
    registerNext.clear();
    for (int i=0;i<LENREGISTER;i++){
        Register rn;
        rn.truename = i;
        rn.V = i+1;
        registerNext.append(rn);
    }
    floatRegisterNext.clear();
    for (int i=0;i<LENREGISTER;i++){
        FRegister frn;
        frn.truename = i + LENREGISTER;
        frn.V = i+1;
        floatRegisterNext.append(frn);
    }
    memoryNext.clear();
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
    init();
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
                addReservationNext[i].inst = inst;
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
                multiplyReservationNext[i].inst = inst;
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
                    multiplyReservationNext[i].VK = F3->V;
                }
                else
                    multiplyReservationNext[i].QK = F3->Q;
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
                bufferNext[i]->IsBusy = true;
                bufferNext[i]->inst = inst;
                bufferNext[i]->trueop = inst->trueop;
                bufferNext[i]->Time = -1;
//                Register *F2 = getRegister(inst->truef2);
//                if (F2->Q == -1) {
//                    bufferNext[i].QJ = -1;
//                    bufferNext[i].VJforBuffer = F2->V;
//                }
//                else
//                    bufferNext[i].QJ = F2->Q;
                bufferNext[i]->A = inst->truef2;
                bufferNext[i]->QK = -1;
                bufferNext[i]->QJ = -1;
                FRenable = true;
                FRres = bufferNext[i]->truename;
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
            if (!bufferNext[i]->IsBusy){
                bufferNext[i]->IsBusy = true;
                bufferNext[i]->inst = inst;
                bufferNext[i]->trueop = inst->trueop;
                bufferNext[i]->Time = -1;
//                Register* F2 = getRegister(inst->truef2);
//                if (F2->Q == -1){
//                    bufferNext[i].QJ = -1;
//                    bufferNext[i].VJforBuffer = F2->V;
//                }
//                else
//                    bufferNext[i].QJ = F2->Q;
                bufferNext[i]->A = inst->truef2;
                bufferNext[i]->Time = -1;
                bufferNext[i]->QJ = -1;
                FRegister *F1 = getFRegister(inst->truef1);
                if (F1->Q == -1){
                    bufferNext[i]->QK = -1;
                    bufferNext[i]->VK = F1->V;
                }
                else
                    bufferNext[i]->QK = F1->Q;
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
                addReservationNext[i].inst ->wbTime = NowCycle;
                addReservationNext[i].IsBusy = false;
            }
            if (addReservationNext[i].QJ == CDBresultReservation){
                addReservationNext[i].QJ = -1;
                addReservationNext[i].VJ = CDBresultData;
            }
            if (addReservationNext[i].QK == CDBresultReservation){
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
                    addReservationNext[i].inst ->startTime = NowCycle;
                    break;
                }
            }
        }
    } else {
        if (AddReservationPointer->Time > 0)
            --AddReservationPointer->Time;
        if (AddReservationPointer->Time == 0){
            AddReservationPointer->inst->completeTime = NowCycle;
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
                            addReservationNext[i].inst ->startTime = NowCycle;
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
                multiplyReservationNext[i].inst ->wbTime = NowCycle;
                multiplyReservationNext[i].IsBusy = false;
            }
            if (multiplyReservationNext[i].QJ == CDBresultReservation){
                multiplyReservationNext[i].QJ = -1;
                multiplyReservationNext[i].VJ = CDBresultData;
            }
            if (multiplyReservationNext[i].QK == CDBresultReservation){
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
                    multiplyReservationNext[i].inst ->startTime = NowCycle;
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
            MultiplyReservationPointer->inst->completeTime = NowCycle;
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
                    if (multiplyReservationNext[i].IsBusy && multiplyReservationNext[i].truename != CDBresultReservationNext) {
                        if (multiplyReservationNext[i].QK == -1 && multiplyReservationNext[i].QJ == -1){
                            MultiplyReservationPointer = &multiplyReservationNext[i];
                            multiplyReservationNext[i].inst ->startTime = NowCycle;
                            switch (MultiplyReservationPointer->trueop){
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
            if (bufferNext[i]->QK == CDBresultReservation){
                bufferNext[i]->QK = -1;
                bufferNext[i]->VK = CDBresultData;
            }
        }
    }
    if (flag)
    {
        bufferNext[0]->inst->wbTime = NowCycle;
        int name = bufferNext[0]->truename;
        delete bufferNext[0];
        bufferNext.erase(bufferNext.begin());
        RStation *n = new RStation();
        n->truename = name;
        n->Time = -1;
        bufferNext.append(n);
        if (BufferPointer)
            BufferPointer = bufferNext[0];
    }
    flag = false;
    if (BufferPointer == NULL){
        if (bufferNext[0]->IsBusy && bufferNext[0]->QK == -1){
            BufferPointer = bufferNext[0];
            BufferPointer->Time = CYCLELOAD;
            BufferPointer -> inst ->startTime = NowCycle;
        }
    }
    else{

        if (BufferPointer->Time > 0) --BufferPointer->Time;
        if (BufferPointer->Time == 0){
//            BufferPointer->A += BufferPointer->VJforBuffer;
            BufferPointer ->inst->completeTime = NowCycle;
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
            BufferPointer = NULL;
            if (bufferNext[1]->IsBusy && bufferNext[1]->QK == -1){
                BufferPointer = bufferNext[1];
                BufferPointer->Time = CYCLELOAD;
                BufferPointer ->inst->startTime = NowCycle;
            }
        }
    }
    return errorcode;
}

int Status::updateFinal() {
    int errorcode = ERRRIGHT;
    NowCycle++;

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
        Buffer[i].truename = bufferNext[i]->truename;
        Buffer[i].IsBusy = bufferNext[i]->IsBusy;
        Buffer[i].trueop = bufferNext[i]->trueop;
        Buffer[i].Time = bufferNext[i]->Time;
        Buffer[i].QJ = bufferNext[i]->QJ;
        Buffer[i].QK = bufferNext[i]->QK;
        Buffer[i].VJ = bufferNext[i]->VJ;
        Buffer[i].VK = bufferNext[i]->VK;
        Buffer[i].A = bufferNext[i]->A;
        Buffer[i].VJforBuffer = bufferNext[i]->VJforBuffer;
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
    if (instructPointer >= InstructList.size()){
        if (!CDBresultEnable && AddReservationPointer==NULL && MultiplyReservationPointer == NULL &&BufferPointer ==NULL) {
            return ERRALREADYEND;
        }
    }
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
        Buffer[i].Name = Loadstoreconvert[Buffer[i].truename];
        Buffer[i].Op = Opconvert[Buffer[i].trueop];
    }
    for (int i=0;i<registerlen;i++){
        IntRegister[i].Name = QString::number(IntRegister[i].truename);
        if (IntRegister[i].Q == -1)
            IntRegister[i].QName = "";
        else
            IntRegister[i].QName = Nameconvert[IntRegister[i].Q];
    }
    for (int i=0;i<registerlen;i++){
        FloatRegister[i].Name = QString::number(FloatRegister[i].truename);
        if (FloatRegister[i].Q == -1)
            FloatRegister[i].QName = "";
        else
            FloatRegister[i].QName = Nameconvert[FloatRegister[i].Q];
    }
    for (int i=0;i<addreservation;i++){
        AddReservation[i].Name = Nameconvert[AddReservation[i].truename];
        AddReservation[i].Op = Opconvert[AddReservation[i].trueop];
        if (AddReservation[i].QJ == -1)
            AddReservation[i].QJName = "";
        else
            AddReservation[i].QJName = Loadstoreconvert[AddReservation[i].QJ];
        if (AddReservation[i].QK == -1)
            AddReservation[i].QKName = "";
        else
            AddReservation[i].QKName = Loadstoreconvert[AddReservation[i].QK];
    }
    for (int i=0;i<multireservation;i++){
        MultiplyReservation[i].Name = Nameconvert[MultiplyReservation[i].truename];
        MultiplyReservation[i].Op = Opconvert[MultiplyReservation[i].trueop];
        if (MultiplyReservation[i].QJ == -1)
            MultiplyReservation[i].QJName = "";
        else
            MultiplyReservation[i].QJName = Loadstoreconvert[MultiplyReservation[i].QJ];
        if (MultiplyReservation[i].QK == -1)
            MultiplyReservation[i].QKName = "";
        else
            MultiplyReservation[i].QKName = Loadstoreconvert[MultiplyReservation[i].QK];

    }
    qDebug()<<"updateNameend";
}
