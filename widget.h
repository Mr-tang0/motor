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


#include<QMessageBox>
#include<QCloseEvent>

#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QMouseEvent>
#include <QScreen>

#include <QFileInfo>
#include <QDir>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    void initUi(double change);

    void setParam(motor myMotor,int index,QString filePath);
    static void delay(int t);
    void saveJson(motor myMotor,int index,QString filePath);
    void refreshUi(motor myMotor,int index);

    void findPort();
    void connectPort();


    void loadlocalmotor(QString filePath);

    void reseiveMessage();
    void decode(QString res);

    void motorset(motor myMotor);
    void motorMove(motor myMotor,bool d);
    void moveTo(motor myMotor,int len);

    void closeEvent(QCloseEvent *);

    QString rootPath;
    QString filepath;
    QString imgpath;
    QString appversion;

    static QSerialPort *myPort;

    QString portName;
    bool openFlag;
    bool findFlag = false;
    static motor mymotor[8];
    QList <QByteArray> sendlist;


    static motor *threadMotor;bool dir;
    static int threadflag;
    static int stopflag;

    int enableFlag = 0;
    int onlinecounter[8] ={0,0,0,0,0,0,0,0};
    bool blackflag = false;
    bool styleflag = false;



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



    void on_BlackandWhite_clicked();

    void on_Clear_clicked();

    void on_Send_clicked();

    void on_Style_clicked();

    void on_About_clicked();

    void on_Help_clicked();

signals:
    void valueChange(int value);
    void resized(double x ,double y);

private:
    Ui::Widget *ui;
protected:
    void resizeEvent(QResizeEvent *event) override
    {
        emit resized(event->size().width(),event->size().height());
    }


};

#endif // WIDGET_H
