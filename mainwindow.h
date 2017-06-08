#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <QMessageBox>
#include "instructdialog.h"
#include "status.h"
#include "showmemory.h"
#include "modifymemory.h"
using namespace std;

#define DO_LOCK() {LOCK=true; if (nowstatus) nowstatus->LOCKER(true); considerlock();}
#define DO_UNLOCK() {LOCK=false; if (nowstatus) nowstatus->LOCKER(false); considerlock();}
#define SET_RUNSTATUS(x) {runstatus=(x); considerrunstatus();}
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void resizeEvent(QResizeEvent *event);
    InstructDialog *idialog;
private slots:
    void donextstep();
    void dopause();
    void dorestart();
    void dolockandstart();
    void docontinue();
    void dohalt();
    void domodifyinstruct();
    void domodifymemory();
    void doshowmemory();
    void doabout();
    void dohelp();
    void doupdate();
    void doload2();
signals:
    void updatememoryshower();
private:
    Ui::MainWindow *ui;
    QAction* nextstepaction;
    QAction* pauseaction;
    QAction* restartaction;
    QAction* lockandstartaction;
    QAction* continueaction;
    QAction* stopaction;
    QAction* haltaction;
    QAction* instructaction;
    QAction* memoryaction;
    QLabel* msgLabel;
    Status* nowstatus;
    ShowMemory* showmemory;
    ModifyMemory* mdialog;
    void considerlock();
    void considerrunstatus();
    void considerstatusbar();
    void consideritem();
    bool LOCK;
    int runstatus;
    int step;
};

#endif // MAINWINDOW_H
