#include "widget.h"
#include "ui_widget.h"

#include <QFuture>
#include <QtConcurrent/QtConcurrent>


QSerialPort *Widget::myPort = new QSerialPort;
motor *Widget::threadMotor = new motor;
int Widget::threadflag = 0;
int Widget::stopflag = 0;
motor Widget::mymotor[8] = {};



Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    findPort();

    for (int i = 0;i<8;i++)
    {

        QPushButton *cwbtn = findChild<QPushButton*>("cw_"+QString::number(i+1));
        QPushButton *ccwbtn = findChild<QPushButton*>("ccw_"+QString::number(i+1));
        QPushButton *stopbtn = findChild<QPushButton*>("stop_"+QString::number(i+1));

        QPushButton *enablebtn = findChild<QPushButton*>("enable_"+QString::number(i+1));
        QPushButton *disablebtn = findChild<QPushButton*>("disable_"+QString::number(i+1));

        QPushButton *setbtn = findChild<QPushButton*>("set_"+QString::number(i+1));
        QPushButton *set0btn = findChild<QPushButton*>("zero_"+QString::number(i+1));

        QPushButton *motorbtn = findChild<QPushButton*>("pushButton_"+QString::number(i+1));

        QObject::connect(motorbtn,&QPushButton::clicked,[=]()
        {
            ui->motorTab->setCurrentIndex(i);
            qDebug()<<"Q";
        });

        QObject::connect(cwbtn,&QPushButton::clicked,[=]()
        {
            mymotor[i].temp = mymotor[i].position.toInt();
            motorMove(mymotor[i],true);
        });
        QObject::connect(ccwbtn,&QPushButton::clicked,[=]()
        {
            mymotor[i].temp = mymotor[i].position.toInt();
            motorMove(mymotor[i],false);
        });

        QObject::connect(stopbtn,&QPushButton::clicked,[=]()
        {
            stopflag = 1;
            sendlist.clear();
            appendMessage(mymotor[i].address.toInt(),"SK");//停止
        });

        QObject::connect(enablebtn,&QPushButton::clicked,[=]()
        {
            appendMessage(mymotor[i].address.toInt(),"ME");//使能
        });

        QObject::connect(disablebtn,&QPushButton::clicked,[=]()
        {
            appendMessage(mymotor[i].address.toInt(),"MD");//非使能
        });
        QObject::connect(setbtn,&QPushButton::clicked,[=]()
        {
            setParam(mymotor[i],i);//设置
        });
        QObject::connect(set0btn,&QPushButton::clicked,[=]()  //运动时禁止使用
        {
            mymotor[i].zero = mymotor[i].realPosition;
            saveJson(mymotor[i],i);
        });
    }

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_refresh_clicked()
{
     findPort();

}

void Widget::on_connect_clicked()
{
    connectPort();

    QObject::connect(myPort,&QSerialPort::readyRead,[=]()
    {
        reseiveMessage();
    });

    for(int i =3;i<5;i++)
    {
        QObject::connect(timer,&QTimer::timeout,[=]()
        {
            appendMessage(i,"IP");
            appendMessage(i,"AL");//警报
        });
        timer->start(500);

    }

    QObject::connect(timer3,&QTimer::timeout,[=]()
    {
        if(!sendlist.isEmpty())
        {
            myPort->write(sendlist.first());
            sendlist.removeFirst();
        }

    });
    timer3->start(100);
}

void Widget::on_disconnect_clicked()
{
    timer->stop();
    timer3->stop();
    myPort->close();

    ui->openfg->setChecked(false);
    openFlag = false;
    ui->statebox->append("已关闭");
}


void Widget::on_move_clicked()
{

    moveTo(mymotor[0],ui->axisx->text().toInt());
    moveTo(mymotor[1],ui->axisy->text().toInt());
}

void Widget::on_move_to_zero_clicked()
{
    moveTo(mymotor[0],0);
    moveTo(mymotor[1],0);

}

void Widget::on_cwmove_pressed()
{
    stopflag = 0;

    int i = ui->motorIndex->currentText().toInt()-1;

    mymotor[i].temp = 1000000;

    motorMove(mymotor[i],true);
}

void Widget::on_cwmove_released()
{
    stopflag = 1;
    sendlist.clear();
}

void Widget::on_ccwmove_pressed()
{
    stopflag = 0;

    int i = ui->motorIndex->currentText().toInt()-1;

    mymotor[i].temp = 1000000;

    motorMove(mymotor[i],false);
}

void Widget::on_ccwmove_released()
{
    stopflag = 1;
    sendlist.clear();
}
