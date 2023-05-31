#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Qt_test.h"

class Qt_test : public QMainWindow
{
    Q_OBJECT

public:
    Qt_test(QWidget *parent = nullptr);
    ~Qt_test();

private:
    Ui::Qt_testClass ui;
};
