#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QRmalpers.h"

class QRmalpers : public QMainWindow
{
    Q_OBJECT

public:
    QRmalpers(QWidget *parent = Q_NULLPTR);

private slots:
    void verifyCoordinates();
    void createCoordinates();
    void showIcon();

private:
    Ui::QRmalpersClass ui;
};
