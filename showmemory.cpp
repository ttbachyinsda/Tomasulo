#include "showmemory.h"
#include "ui_showmemory.h"

ShowMemory::ShowMemory(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowMemory)
{
    ui->setupUi(this);
}

ShowMemory::~ShowMemory()
{
    delete ui;
}

void ShowMemory::update()
{
    ui->listWidget->clear();
    for (int i=0;i<memorylen;i++){
        if (fabs(nowstatus->Memory[i]-0.0)>1e-9)
            ui->listWidget->addItem("base+"+QString::number(i)+" : "+QString("%1").arg(nowstatus->Memory[i]));
    }
}
