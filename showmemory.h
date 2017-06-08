#ifndef SHOWMEMORY_H
#define SHOWMEMORY_H

#include <QDialog>
#include <QMessageBox>
#include "status.h"
#include <iostream>
#include <QDebug>
#include <vector>
#include <QKeyEvent>

namespace Ui {
class ShowMemory;
}

class ShowMemory : public QDialog
{
    Q_OBJECT

public:
    explicit ShowMemory(QWidget *parent = 0);
    ~ShowMemory();
    Status* nowstatus;
public slots:
    void update();

private:
    Ui::ShowMemory *ui;
};

#endif // SHOWMEMORY_H
