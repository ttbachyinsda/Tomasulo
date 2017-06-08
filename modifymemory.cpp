#include "modifymemory.h"
#include "ui_modifymemory.h"

ModifyMemory::ModifyMemory(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModifyMemory)
{
    ui->setupUi(this);
    connect(ui->modifyaction,SIGNAL(clicked()),this,SLOT(modifyitem()));
    connect(ui->cancel,SIGNAL(clicked(bool)),this,SLOT(cancel()));
    connect(this,SIGNAL(accepted()),this,SLOT(updateitem()));
}

ModifyMemory::~ModifyMemory()
{
    delete ui;
}

void ModifyMemory::update()
{
    tempvec.clear();
    for (int i=0;i<memorylen;i++)
        tempvec.append(nowstatus->Memory[i]);
    ui->listWidget->clear();
    for (int i=0;i<memorylen;i++){
        if (fabs(tempvec[i]-0.0)>1e-9)
            ui->listWidget->addItem("base+"+QString::number(i)+" : "+QString("%1").arg(tempvec[i]));
    }
}

void ModifyMemory::cancel()
{
    int result = QMessageBox::question(this, "提示", "是否退出编辑？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    if (result == QMessageBox::Yes){
        this->close();
    }
}
//IMPORTANT
//在这里，需要设置内存设置的规则
void ModifyMemory::modifyitem()
{
    QString position = ui->position->text();
    QString value = ui->value->text();
    bool ok1,ok2;
    int num1 = position.toInt(&ok1);
    float num2 = value.toFloat(&ok2);
    if (ok1 && ok2){
        if (num1>=0 && num1<memorylen){
            tempvec[num1] = num2;
            ui->listWidget->clear();
            for (int i=0;i<memorylen;i++){
                if (fabs(tempvec[i]-0.0)>1e-9)
                    ui->listWidget->addItem("base+"+QString::number(i)+" : "+QString("%1").arg(tempvec[i]));
            }
            return;
        }
    }
    QMessageBox::critical(this,"警告","输入指令格式有误");
}

void ModifyMemory::updateitem()
{
    nowstatus->Memory.clear();
    nowstatus->memoryNext.clear();
    for (int i=0;i<memorylen;i++)
    {
        nowstatus->Memory.append(tempvec[i]);
        nowstatus->memoryNext.append(tempvec[i]);
    }
    emit updatemain();
}
void ModifyMemory::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
    {
        //cout<<"add item"<<endl;
        modifyitem();
    }
}
