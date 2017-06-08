#ifndef INSTRUCTDIALOG_H
#define INSTRUCTDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include "status.h"
#include <iostream>
#include <QDebug>
#include <vector>
#include <map>
#include <QKeyEvent>
using namespace std;

namespace Ui {
class InstructDialog;
}

class InstructDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InstructDialog(QWidget *parent = 0);
    ~InstructDialog();
    void update();
    Status* nowstatus;
    void keyPressEvent(QKeyEvent *e);
public slots:
    void additem();
    void deleteitem();
    void cancel();
    void updateitem();

signals:
    void updatemain();

private:
    Ui::InstructDialog *ui;
    pair<QString,bool> validate(QString instruction);
    map<QString,int> registerconvert;
};

#endif // INSTRUCTDIALOG_H
