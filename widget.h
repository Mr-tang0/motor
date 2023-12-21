#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include<QtSerialPort/QSerialPort>
#include<QtSerialPort/QSerialPortInfo>
#include "ui_widget.h"
#include "setui.h"
#include "ui_setui.h"
#include<QRegExp>
#include<QFile>
#include<QDebug>
#include"motor.h"
#include <QTimer>
#include<QObject>

#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonArray>

#include <QThread>


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    void setParam(motor myMotor,int index);
    static void delay(int t);
    void saveJson(motor myMotor,int index);
    void refreshUi();

    void findPort();
    void connectPort();


    void reseiveMessage();
    void decode(QString res);

    void motorset(motor myMotor);
    void motorMove(motor myMotor,bool d);
    void moveTo(motor myMotor,int len);

    static QSerialPort *myPort;

    QString portName;
    bool openFlag;   
    static motor mymotor[8];
    QList <QByteArray> sendlist;


    static motor *threadMotor;bool dir;
    static int threadflag;
    static int stopflag;

    int enableFlag = 0;
    QString rootPath;
    QTimer *timer = new QTimer(this);
    QTimer *timer2 = new QTimer(this);
    QTimer *timer3 = new QTimer(this);

private slots:   
    void appendMessage(const int &index,const QString &data);
    void prependMessage(const int &index,const QString &data);

    void on_refresh_clicked();

    void on_connect_clicked();

    void on_disconnect_clicked();

    void on_move_clicked();

    void on_move_to_zero_clicked();

    void on_cwmove_pressed();

    void on_cwmove_released();

    void on_ccwmove_pressed();

    void on_ccwmove_released();

signals:
    void valueChange(int value);

private:
    Ui::Widget *ui;

};

#endif // WIDGET_H
