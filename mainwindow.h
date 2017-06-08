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
    QTableWidget *instList, *regList, *memList, *rsList;
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
    void nextInst();
    void continueInst();
    void multiInst();
    void showAll();
    void showNZ();
    void changeMemory(int r, int c);
};

#endif // MAINWINDOW_H
