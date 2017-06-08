#ifndef MODIFYMEMORY_H
#define MODIFYMEMORY_H

#include <QDialog>
#include <QMessageBox>
#include "status.h"
#include <iostream>
#include <QDebug>
#include <vector>
#include <QKeyEvent>
using namespace std;

namespace Ui {
class ModifyMemory;
}

class ModifyMemory : public QDialog
{
    Q_OBJECT

public:
    explicit ModifyMemory(QWidget *parent = 0);
    ~ModifyMemory();
    void update();
    Status* nowstatus;
    void keyPressEvent(QKeyEvent *e);

public slots:
    void modifyitem();
    void cancel();
    void updateitem();

signals:
    void updatemain();

private:
    Ui::ModifyMemory *ui;
    QVector<float> tempvec;
};

#endif // MODIFYMEMORY_H
