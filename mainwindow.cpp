#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //设置图标
    this->setWindowIcon(QIcon(":/icon.ico"));
    //toolbar的初始化

    instructaction = new QAction(QIcon(":/document.ico"),"修改指令",this);
    ui->mainToolBar->addAction(instructaction);
    connect(instructaction,SIGNAL(triggered(bool)),this,SLOT(domodifyinstruct()));
    connect(ui->actionmodifyinstruct,SIGNAL(triggered(bool)),this,SLOT(domodifyinstruct()));

    memoryaction = new QAction(QIcon(":/memory.ico"),"修改内存",this);
    ui->mainToolBar->addAction(memoryaction);
    connect(memoryaction,SIGNAL(triggered(bool)),this,SLOT(domodifymemory()));
    connect(ui->actionmodifymemory,SIGNAL(triggered(bool)),this,SLOT(domodifymemory()));

    lockandstartaction = new QAction(QIcon(":/lock.ico"),"锁定并开始模拟",this);
    ui->mainToolBar->addAction(lockandstartaction);
    connect(lockandstartaction,SIGNAL(triggered(bool)),this,SLOT(dolockandstart()));
    connect(ui->actionLockandStart,SIGNAL(triggered(bool)),this,SLOT(dolockandstart()));

    nextstepaction = new QAction(QIcon(":/nextstep.ico"),"执行下一步",this);
    ui->mainToolBar->addAction(nextstepaction);
    connect(nextstepaction,SIGNAL(triggered(bool)),this,SLOT(donextstep()));
    connect(ui->actionnextStep,SIGNAL(triggered(bool)),this,SLOT(donextstep()));

    continueaction = new QAction(QIcon(":/run.ico"),"自动执行",this);
    ui->mainToolBar->addAction(continueaction);
    connect(continueaction,SIGNAL(triggered(bool)),this,SLOT(docontinue()));
    connect(ui->actionautorun,SIGNAL(triggered(bool)),this,SLOT(docontinue()));

    pauseaction = new QAction(QIcon(":/pause.ico"),"暂停自动执行",this);
    ui->mainToolBar->addAction(pauseaction);
    connect(pauseaction,SIGNAL(triggered(bool)),this,SLOT(dopause()));
    connect(ui->actionpause,SIGNAL(triggered(bool)),this,SLOT(dopause()));

    restartaction = new QAction(QIcon(":/restart.ico"),"重新执行模拟",this);
    ui->mainToolBar->addAction(restartaction);
    connect(restartaction,SIGNAL(triggered(bool)),this,SLOT(dorestart()));
    connect(ui->actionrestart,SIGNAL(triggered(bool)),this,SLOT(dorestart()));

    haltaction = new QAction(QIcon(":/halt.ico"),"终止模拟并解除锁定",this);
    ui->mainToolBar->addAction(haltaction);
    connect(haltaction,SIGNAL(triggered(bool)),this,SLOT(dohalt()));
    connect(ui->actionHalt,SIGNAL(triggered(bool)),this,SLOT(dohalt()));

    //end toolbar


    //ui中关于和帮助的连接
    connect(ui->actionabout,SIGNAL(triggered(bool)),this,SLOT(doabout()));
    connect(ui->actionhelp,SIGNAL(triggered(bool)),this,SLOT(dohelp()));
    connect(ui->actionLoad2,SIGNAL(triggered(bool)),this,SLOT(doload2()));
    //end other actions
    nowstatus = new Status();
    //IMPORTANT
    //如果要修改表格的显示请在下面修改。其中
    //ui->instructionqueue->setColumnCount(8); 修改列数，setRowCount修改行数
    //ui->instructionqueue->setHorizontalHeaderLabels(insheader); 修改表头

    //表格的表头和行号和列号在下面修改
    ui->horizontalLayoutWidget->resize(this->width(),this->height()-ui->mainToolBar->height()-ui->statusBar->height()-ui->menuBar->height());
    ui->instructionqueue->setColumnCount(7);
    ui->instructionqueue->setRowCount(max(10,nowstatus->InstructList.length()));
    QStringList insheader;
    insheader<<"指令名称"<<"操作数1"<<"操作数2"<<"操作数3"<<"开始时间"<<"完成时间"<<"写回时间";
    ui->instructionqueue->setHorizontalHeaderLabels(insheader);
    ui->reservestation->setColumnCount(8);
    ui->reservestation->setRowCount(addreservation+multireservation);
    QStringList resheader;
    resheader<<"剩余时间"<<"名称"<<"运行状态"<<"运算符"<<"VJ"<<"VK"<<"QJ"<<"QK";
    ui->reservestation->setHorizontalHeaderLabels(resheader);
    ui->reservestation->setMinimumHeight((ui->reservestation->rowCount()+1)*ui->reservestation->rowHeight(0));
    ui->FU->setColumnCount(registerlen);
    ui->FU->setRowCount(2);
    ui->FU->setMinimumHeight((ui->FU->rowCount()+1)*ui->FU->rowHeight(0));
    QStringList fheader;
    for (int i=0;i<ui->FU->columnCount();i++){
        fheader<<"F"+QString::number(i);
    }
    QStringList fverheader;
    fverheader<<"Value"<<"Pointer";
    ui->FU->setHorizontalHeaderLabels(fheader);
    ui->FU->setVerticalHeaderLabels(fverheader);
    ui->loadstorequeue->setColumnCount(6);
    ui->loadstorequeue->setRowCount(bufferlen);
    QStringList loadheader;
    loadheader<<"名称"<<"运行状态"<<"地址"<<"运行时间"<<"VK"<<"QK";
    ui->loadstorequeue->setHorizontalHeaderLabels(loadheader);
    for (int i=0;i<ui->instructionqueue->columnCount();i++){
        ui->instructionqueue->setColumnWidth(i,60);
    }
    for (int i=0;i<ui->reservestation->columnCount();i++){
        ui->reservestation->setColumnWidth(i,60);
    }
    for (int i=0;i<ui->FU->columnCount();i++){
        ui->FU->setColumnWidth(i,60);
    }
    for (int i=0;i<ui->loadstorequeue->columnCount();i++){
        ui->loadstorequeue->setColumnWidth(i,60);
    }
    //end content

    //解锁，使ui可以修改
    DO_UNLOCK();
    //end consider

    //初始化状态栏
    msgLabel = new QLabel();
    msgLabel->setMinimumSize(msgLabel->sizeHint());
    msgLabel->setAlignment(Qt::AlignHCenter);
    ui->statusBar->addWidget(msgLabel);
    //end initialize

    //初始化运行状态
    step = 0;
    SET_RUNSTATUS(0);
    consideritem();
    //end initialize

    //初始化显示内存的窗口
    showmemory = NULL;
    connect(ui->showmemory,SIGNAL(pressed()),this,SLOT(doshowmemory()));
    //end initialize
}
//析构函数
MainWindow::~MainWindow()
{
    delete ui;
}
//界面适配
void MainWindow::resizeEvent(QResizeEvent *event)
{
    ui->horizontalLayoutWidget->resize(this->width(),this->height()-ui->mainToolBar->height()-ui->statusBar->height()-ui->menuBar->height());
}
//IMPORTANT
//需要实现以下几个do打头的函数。实现范例请看donextstep的注释
void MainWindow::donextstep()
{
    //核心功能，必须实现
    step++;
    cout<<"do next step"<<endl;
    SET_RUNSTATUS(1);
    int errorcode = nowstatus->nextstep();
    if (errorcode == ERRALREADYEND) SET_RUNSTATUS(2);
    doupdate();

    //如果运行结束，需要SET_RUNSTATUS(2);
    //if (runstatus == RUN_END) SET_RUNSTATUS(2);
}
void MainWindow::dolockandstart()
{
    //正常情况不需要修改这个函数
    cout<<"do lock and start"<<endl;
    DO_LOCK();
    SET_RUNSTATUS(0);
}
void MainWindow::docontinue()
{
    //需要实现一个计时器，就差这个功能的时候可以来找我实现
    cout<<"do continue"<<endl;
    SET_RUNSTATUS(3);
}
void MainWindow::dopause()
{
    //需要实现一个计时器，就差这个功能的时候可以来找我实现
    cout<<"do pause"<<endl;
    SET_RUNSTATUS(1);
}
void MainWindow::dorestart()
{
    //需要实现从头开始的函数，如：
    nowstatus->restart();
    step = 0;
    cout<<"do restart"<<endl;
    SET_RUNSTATUS(0);
    doupdate();
}
void MainWindow::dohalt()
{
    //此时会解锁，可以使用清除所有内容或者不作任何处理两种策略，但无论如何需要将status中的其余内容置零。可以与restart作同一实现
    cout<<"do halt"<<endl;
    nowstatus->restart();
    step = 0;
    DO_UNLOCK();
    SET_RUNSTATUS(0);
    doupdate();
}
void MainWindow::doabout()
{
    //不需要修改，这是关于界面的显示
    cout<<"do about"<<endl;
    QMessageBox::information(this, tr("关于"), "Powered By ttbachyinsda, Fate Faker and Fearless.");
}
void MainWindow::dohelp()
{
    //不需要修改，这是帮助界面的显示
    cout<<"do help"<<endl;
    QMessageBox::information(this, tr("帮助"), "再写帮助就没时间刷恍惚套了");
}

void MainWindow::domodifyinstruct()
{
    //不需要修改，这是修改指令界面的显示
    cout<<"do modify instruct"<<endl;
    idialog = new InstructDialog(this);
    idialog->nowstatus = nowstatus;
    idialog->update();
    disconnect(idialog,SIGNAL(updatemain()),this,SLOT(doupdate()));
    connect(idialog,SIGNAL(updatemain()),this,SLOT(doupdate()));
    idialog->setWindowModality(Qt::ApplicationModal);
    idialog->show();
}
void MainWindow::domodifymemory()
{
    //不需要修改，这是修改内存值界面的显示
    cout<<"do modify memory"<<endl;
    mdialog = new ModifyMemory(this);
    mdialog->nowstatus = nowstatus;
    mdialog->update();
    disconnect(mdialog,SIGNAL(updatemain()),this,SLOT(doupdate()));
    connect(mdialog,SIGNAL(updatemain()),this,SLOT(doupdate()));
    mdialog->setWindowModality(Qt::ApplicationModal);
    mdialog->show();
}
void MainWindow::doshowmemory()
{
    //不需要修改，这是显示内存值界面的显示
    cout<<"do show memory"<<endl;
    if (showmemory == NULL){
        showmemory = new ShowMemory(this);
        showmemory->nowstatus = nowstatus;
        showmemory->update();
        disconnect(this,SIGNAL(updatememoryshower()),showmemory,SLOT(update()));
        connect(this,SIGNAL(updatememoryshower()),showmemory,SLOT(update()));
        showmemory->show();
    } else {
        showmemory->update();
        disconnect(this,SIGNAL(updatememoryshower()),showmemory,SLOT(update()));
        connect(this,SIGNAL(updatememoryshower()),showmemory,SLOT(update()));
        showmemory->show();
    }
}
//这里是对于界面是否可用的判断。注意主界面所有元素均不可以修改。这里不需要修改。
void MainWindow::considerlock()
{
    if (LOCK == false){
        ui->menu->setEnabled(true);
        ui->actionmodifyinstruct->setEnabled(true);
        instructaction->setEnabled(true);
        ui->actionmodifymemory->setEnabled(true);
        memoryaction->setEnabled(true);
        ui->actionLockandStart->setEnabled(true);
        lockandstartaction->setEnabled(true);
    } else {
        ui->menu->setEnabled(false);
        ui->actionmodifyinstruct->setEnabled(false);
        instructaction->setEnabled(false);
        ui->actionmodifymemory->setEnabled(false);
        memoryaction->setEnabled(false);
        ui->actionLockandStart->setEnabled(false);
        lockandstartaction->setEnabled(false);
    }
}
//这里是对于运行状态的判断，调用SET_RUNSTATUS(runstatus)的时候会自动调用这里
void MainWindow::considerrunstatus()
{
    if (LOCK == false){
        ui->actionnextStep->setEnabled(false);
        ui->actionautorun->setEnabled(false);
        ui->actionHalt->setEnabled(false);
        ui->actionpause->setEnabled(false);
        ui->actionrestart->setEnabled(false);
        nextstepaction->setEnabled(false);
        continueaction->setEnabled(false);
        haltaction->setEnabled(false);
        pauseaction->setEnabled(false);
        restartaction->setEnabled(false);
    } else{
        if (runstatus == 0){
            ui->actionnextStep->setEnabled(true);
            ui->actionautorun->setEnabled(true);
            ui->actionHalt->setEnabled(true);
            ui->actionpause->setEnabled(false);
            ui->actionrestart->setEnabled(false);
            nextstepaction->setEnabled(true);
            continueaction->setEnabled(true);
            haltaction->setEnabled(true);
            pauseaction->setEnabled(false);
            restartaction->setEnabled(false);
        } else if (runstatus == 1){
            ui->actionnextStep->setEnabled(true);
            ui->actionautorun->setEnabled(true);
            ui->actionHalt->setEnabled(true);
            ui->actionpause->setEnabled(false);
            ui->actionrestart->setEnabled(true);
            nextstepaction->setEnabled(true);
            continueaction->setEnabled(true);
            haltaction->setEnabled(true);
            pauseaction->setEnabled(false);
            restartaction->setEnabled(true);
        } else if (runstatus == 2){
            ui->actionnextStep->setEnabled(false);
            ui->actionautorun->setEnabled(false);
            ui->actionHalt->setEnabled(true);
            ui->actionpause->setEnabled(false);
            ui->actionrestart->setEnabled(true);
            nextstepaction->setEnabled(false);
            continueaction->setEnabled(false);
            haltaction->setEnabled(true);
            pauseaction->setEnabled(false);
            restartaction->setEnabled(true);
        } else if (runstatus == 3){
            ui->actionnextStep->setEnabled(false);
            ui->actionautorun->setEnabled(false);
            ui->actionHalt->setEnabled(true);
            ui->actionpause->setEnabled(true);
            ui->actionrestart->setEnabled(true);
            nextstepaction->setEnabled(false);
            continueaction->setEnabled(false);
            haltaction->setEnabled(true);
            pauseaction->setEnabled(true);
            restartaction->setEnabled(true);
        }
    }
    considerstatusbar();
}
//这里是对于状态栏的处理，状态的显示默认在状态栏中，如有需要请联系我。调用SET_RUNSTATUS(runstatus)的时候会自动调用这里。
void MainWindow::considerstatusbar()
{
    QString totaltext;
    if (LOCK == true){
        this->setWindowTitle("Tomasulo simulator (LOCKED)");
        totaltext += "模拟器已处于锁定状态，";
    } else {
        this->setWindowTitle("Tomasulo simulator");
        totaltext += "模拟器处于可编辑状态，";
    }
    if (runstatus == 0){
        totaltext += "当前尚未运行中。";
    } else if (runstatus == 1) {
        totaltext += "当前正在运行中，已运行"+QString::number(step)+"步。";
    } else if (runstatus == 3) {
        totaltext += "当前正在自动运行中，已运行"+QString::number(step)+"步。";
    } else {
        totaltext += "当前运行完毕，共运行"+QString::number(step)+"步。";
    }
    msgLabel->setText(totaltext);
}

//IMPORTANT
//如果修改了表格的显示那么必须实现，这里根据nowstatus的值来更新表格内容。
void MainWindow::consideritem()
{
    emit updatememoryshower();
    ui->instructionqueue->setColumnCount(7);
    ui->instructionqueue->setRowCount(max(10,nowstatus->InstructList.length()));
    QStringList insheader;
    insheader<<"指令名称"<<"操作数1"<<"操作数2"<<"操作数3"<<"开始时间"<<"完成时间"<<"写回时间";
    ui->instructionqueue->setHorizontalHeaderLabels(insheader);
    for (int i=0;i<ui->instructionqueue->rowCount();i++){
        ui->instructionqueue->setItem(i,0,new QTableWidgetItem(""));
        ui->instructionqueue->setItem(i,1,new QTableWidgetItem(""));
        ui->instructionqueue->setItem(i,2,new QTableWidgetItem(""));
        ui->instructionqueue->setItem(i,3,new QTableWidgetItem(""));
        ui->instructionqueue->setItem(i,4,new QTableWidgetItem(""));
        ui->instructionqueue->setItem(i,5,new QTableWidgetItem(""));
        ui->instructionqueue->setItem(i,6,new QTableWidgetItem(""));
    }
    //指令队列的显示在下面一个循环
    for (int i=0;i<nowstatus->InstructList.size();i++){
        ui->instructionqueue->setItem(i,0,new QTableWidgetItem(nowstatus->InstructList[i].Op));
        ui->instructionqueue->setItem(i,1,new QTableWidgetItem(nowstatus->InstructList[i].F1));
        ui->instructionqueue->setItem(i,2,new QTableWidgetItem(nowstatus->InstructList[i].F2));
        ui->instructionqueue->setItem(i,3,new QTableWidgetItem(nowstatus->InstructList[i].F3));
        if (nowstatus->InstructList[i].startTime == -1)
            ui->instructionqueue->setItem(i,4,new QTableWidgetItem(""));
        else
            ui->instructionqueue->setItem(i,4,new QTableWidgetItem(QString::number(nowstatus->InstructList[i].startTime)));
        if (nowstatus->InstructList[i].completeTime == -1)
            ui->instructionqueue->setItem(i,5,new QTableWidgetItem(""));
        else
            ui->instructionqueue->setItem(i,5,new QTableWidgetItem(QString::number(nowstatus->InstructList[i].completeTime)));
        if (nowstatus->InstructList[i].wbTime == -1)
            ui->instructionqueue->setItem(i,6,new QTableWidgetItem(""));
        else
            ui->instructionqueue->setItem(i,6,new QTableWidgetItem(QString::number(nowstatus->InstructList[i].wbTime)));
        for (int k=0;k<=6;k++){
            ui->instructionqueue->item(i,k)->setTextAlignment(Qt::AlignCenter);
        }
    }
    //保留栈的显示在下面两个循环
    for (int i=0;i<addreservation;i++){
        if (nowstatus->AddReservation[i].Time == -1)
            ui->reservestation->setItem(i,0,new QTableWidgetItem(""));
        else
            ui->reservestation->setItem(i,0,new QTableWidgetItem(QString::number(nowstatus->AddReservation[i].Time)));
        ui->reservestation->setItem(i,1,new QTableWidgetItem(nowstatus->AddReservation[i].Name));
        if (nowstatus->AddReservation[i].IsBusy){
            ui->reservestation->setItem(i,2,new QTableWidgetItem("Busy"));
            ui->reservestation->item(i,2)->setTextColor(Qt::red);
        } else {
            ui->reservestation->setItem(i,2,new QTableWidgetItem("Free"));
            ui->reservestation->item(i,2)->setTextColor(Qt::green);
        }
        ui->reservestation->setItem(i,3,new QTableWidgetItem(nowstatus->AddReservation[i].Op));
        ui->reservestation->setItem(i,4,new QTableWidgetItem(QString("%1").arg(nowstatus->AddReservation[i].VJ)));
        ui->reservestation->setItem(i,5,new QTableWidgetItem(QString("%1").arg(nowstatus->AddReservation[i].VK)));
        ui->reservestation->setItem(i,6,new QTableWidgetItem(nowstatus->AddReservation[i].QJName));
        ui->reservestation->setItem(i,7,new QTableWidgetItem(nowstatus->AddReservation[i].QKName));
        for (int k=0;k<=7;k++){
            ui->reservestation->item(i,k)->setTextAlignment(Qt::AlignCenter);
        }
    }
    for (int i=0;i<multireservation;i++){
        int j = i + addreservation;
        if (nowstatus->MultiplyReservation[i].Time == -1)
            ui->reservestation->setItem(j,0,new QTableWidgetItem(""));
        else
            ui->reservestation->setItem(j,0,new QTableWidgetItem(QString::number(nowstatus->MultiplyReservation[i].Time)));
        ui->reservestation->setItem(j,1,new QTableWidgetItem(nowstatus->MultiplyReservation[i].Name));
        if (nowstatus->MultiplyReservation[i].IsBusy){
            ui->reservestation->setItem(j,2,new QTableWidgetItem("Busy"));
            ui->reservestation->item(j,2)->setTextColor(Qt::red);
        } else {
            ui->reservestation->setItem(j,2,new QTableWidgetItem("Free"));
            ui->reservestation->item(j,2)->setTextColor(Qt::green);
        }
        ui->reservestation->setItem(j,3,new QTableWidgetItem(nowstatus->MultiplyReservation[i].Op));
        ui->reservestation->setItem(j,4,new QTableWidgetItem(QString("%1").arg(nowstatus->MultiplyReservation[i].VJ)));
        ui->reservestation->setItem(j,5,new QTableWidgetItem(QString("%1").arg(nowstatus->MultiplyReservation[i].VK)));
        ui->reservestation->setItem(j,6,new QTableWidgetItem(nowstatus->MultiplyReservation[i].QJName));
        ui->reservestation->setItem(j,7,new QTableWidgetItem(nowstatus->MultiplyReservation[i].QKName));
        for (int k=0;k<=7;k++){
            ui->reservestation->item(j,k)->setTextAlignment(Qt::AlignCenter);
        }
    }
    //浮点寄存器的显示
    for (int i=0;i<registerlen;i++){
        ui->FU->setItem(0,i,new QTableWidgetItem(QString("%1").arg(nowstatus->FloatRegister[i].V)));
        ui->FU->item(0,i)->setTextAlignment(Qt::AlignCenter);
        ui->FU->setItem(1,i,new QTableWidgetItem(nowstatus->FloatRegister[i].QName));
        ui->FU->item(1,i)->setTextAlignment(Qt::AlignCenter);
    }
    //load queue的显示
    for (int i=0;i<bufferlen;i++){
        ui->loadstorequeue->setItem(i,0,new QTableWidgetItem(nowstatus->Buffer[i].Name));
        ui->loadstorequeue->item(i,0)->setTextAlignment(Qt::AlignCenter);
        if (nowstatus->Buffer[i].IsBusy){
            ui->loadstorequeue->setItem(i,1,new QTableWidgetItem("Busy"));
            ui->loadstorequeue->item(i,1)->setTextColor(Qt::red);
            ui->loadstorequeue->item(i,1)->setTextAlignment(Qt::AlignCenter);
        } else {
            ui->loadstorequeue->setItem(i,1,new QTableWidgetItem("Free"));
            ui->loadstorequeue->item(i,1)->setTextColor(Qt::green);
            ui->loadstorequeue->item(i,1)->setTextAlignment(Qt::AlignCenter);
        }
        ui->loadstorequeue->setItem(i,2,new QTableWidgetItem(QString::number(nowstatus->Buffer[i].A)));
        ui->loadstorequeue->item(i,2)->setTextAlignment(Qt::AlignCenter);
        if (nowstatus->Buffer[i].Time == -1)
            ui->loadstorequeue->setItem(i,3,new QTableWidgetItem(""));
        else
            ui->loadstorequeue->setItem(i,3,new QTableWidgetItem(QString::number(nowstatus->Buffer[i].Time)));
        ui->loadstorequeue->item(i,3)->setTextAlignment(Qt::AlignCenter);
        if (nowstatus->Buffer[i].VK == -1)
            ui->loadstorequeue->setItem(i,4,new QTableWidgetItem(""));
        else
            ui->loadstorequeue->setItem(i,4,new QTableWidgetItem(QString::number(nowstatus->Buffer[i].VK)));
        ui->loadstorequeue->item(i,4)->setTextAlignment(Qt::AlignCenter);
        ui->loadstorequeue->setItem(i,5,new QTableWidgetItem(nowstatus->Buffer[i].QKName));
        ui->loadstorequeue->item(i,5)->setTextAlignment(Qt::AlignCenter);
    }
}
//这是一个回调函数，不需要修改
void MainWindow::doupdate()
{
    cout<<"do update"<<endl;
    consideritem();
}

void MainWindow::doload2()
{
    nowstatus->InstructList.clear();
    Instruction i[10];
    i[0].Op = "LD";
    i[0].F1 = "F1";
    i[0].F2 = "25";
    i[0].F3 = "";
    i[1].Op = "LD";
    i[1].F1 = "F2";
    i[1].F2 = "30";
    i[1].F3 = "";
    i[2].Op = "ADDD";
    i[2].F1 = "F3";
    i[2].F2 = "F1";
    i[2].F3 = "F2";
    i[3].Op = "MULD";
    i[3].F1 = "F1";
    i[3].F2 = "F2";
    i[3].F3 = "F4";
    i[4].Op = "ST";
    i[4].F1 = "F3";
    i[4].F2 = "50";
    i[4].F3 = "";
    i[5].Op = "LD";
    i[5].F1 = "F1";
    i[5].F2 = "25";
    i[5].F3 = "";
    i[6].Op = "LD";
    i[6].F1 = "F2";
    i[6].F2 = "30";
    i[6].F3 = "";
    i[7].Op = "ADDD";
    i[7].F1 = "F3";
    i[7].F2 = "F1";
    i[7].F3 = "F2";
    i[8].Op = "MULD";
    i[8].F1 = "F1";
    i[8].F2 = "F2";
    i[8].F3 = "F4";
    i[9].Op = "ST";
    i[9].F1 = "F3";
    i[9].F2 = "50";
    i[9].F3 = "";
    for (int j=0;j<10;j++){
        Instruction instruction = i[j];
        if (instruction.Op == "LD" || instruction.Op == "ST"){
            instruction.trueop = nowstatus->registerconvert[instruction.Op];
            instruction.truef1 = nowstatus->registerconvert[instruction.F1];
            instruction.truef2 = instruction.F2.toInt();
        } else {
            instruction.trueop = nowstatus->registerconvert[instruction.Op];
            instruction.truef1 = nowstatus->registerconvert[instruction.F1];
            instruction.truef2 = nowstatus->registerconvert[instruction.F2];
            instruction.truef3 = nowstatus->registerconvert[instruction.F3];
        }
        nowstatus->InstructList.append(instruction);
    }
    doupdate();
}

void MainWindow::doload1()
{
    nowstatus->InstructList.clear();
    Instruction i[10];
    i[0].Op = "ADDD";
    i[0].F1 = "F1";
    i[0].F2 = "F1";
    i[0].F3 = "F1";
    i[1].Op = "ST";
    i[1].F1 = "F!";
    i[1].F2 = "10";
    i[1].F3 = "";
    i[2].Op = "SUBD";
    i[2].F1 = "F1";
    i[2].F2 = "F1";
    i[2].F3 = "F2";
    i[3].Op = "LD";
    i[3].F1 = "F2";
    i[3].F2 = "50";
    i[3].F3 = "";
    i[4].Op = "MULD";
    i[4].F1 = "F3";
    i[4].F2 = "F1";
    i[4].F3 = "F2";
    i[5].Op = "MULD";
    i[5].F1 = "F3";
    i[5].F2 = "F1";
    i[5].F3 = "F2";
    i[6].Op = "ST";
    i[6].F1 = "F3";
    i[6].F2 = "50";
    i[6].F3 = "";
    i[7].Op = "LD";
    i[7].F1 = "F3";
    i[7].F2 = "50";
    i[7].F3 = "";
    i[8].Op = "DIVD";
    i[8].F1 = "F3";
    i[8].F2 = "F2";
    i[8].F3 = "F3";
    i[9].Op = "ST";
    i[9].F1 = "F3";
    i[9].F2 = "50";
    i[9].F3 = "";
    for (int j=0;j<10;j++){
        Instruction instruction = i[j];
        if (instruction.Op == "LD" || instruction.Op == "ST"){
            instruction.trueop = nowstatus->registerconvert[instruction.Op];
            instruction.truef1 = nowstatus->registerconvert[instruction.F1];
            instruction.truef2 = instruction.F2.toInt();
        } else {
            instruction.trueop = nowstatus->registerconvert[instruction.Op];
            instruction.truef1 = nowstatus->registerconvert[instruction.F1];
            instruction.truef2 = nowstatus->registerconvert[instruction.F2];
            instruction.truef3 = nowstatus->registerconvert[instruction.F3];
        }
        nowstatus->InstructList.append(instruction);
    }
    doupdate();
}
