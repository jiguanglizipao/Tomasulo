#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QLCDNumber>
#include <QSpinBox>
#include "tomasulo.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    QTableWidget *instList, *regList, *memList, *rsList, *lsList;
    QLineEdit *instEdit;
    QSpinBox *numSpin;
    QLCDNumber *cycleNumber;
    Tomasulo *tomasulo;
    bool showall;

private slots:
    void addInst(const QString *str = NULL);
    void loadInst();
    void updateInstList();
    void updateCycleNumber();
    void updateMemory();
    void updateRegister();
    void updateRStation();
    void updateLSQueue();
    void nextInst();
    void continueInst();
    void multiInst();
    void showAll();
    void showNZ();
    void changeMemory(int r, int c);
    void modifyMemory();
};


class MySpinBox: public QSpinBox
{
    Q_OBJECT

public:
    MySpinBox( QWidget * parent = 0) :
        QSpinBox(parent)
    {
    }

    virtual QString textFromValue ( int value ) const
    {
        /* 4 - number of digits, 10 - base of number, '0' - pad character*/
        return QString("%1").arg(value, 4 , 10, QChar('0'));
    }
};


#endif // MAINWINDOW_H
