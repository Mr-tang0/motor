#include "movework.h"

movework::movework(QObject *parent) : QObject(parent)
{

}

void movework::motorMove()
{
    int address = Widget::threadMotor->address.toInt();
    int len = Widget::threadMotor->temp;
    QString moveLength = QString::number(10000);

    for (int i  = 0;i<int(len/10000);i++)
    {
        if (Widget::stopflag)break;
        emit sendData(address,"FL"+moveLength);
        emit sendData(address,"IP");
        emit sendData(address,"AL");
        emit sendData(address,"RS");

        delay(400);
    }
    if(len%10000!=0 and Widget::stopflag!=1)
    {
        while(Widget::threadflag==1){};
        moveLength = QString::number(len%10000);
        emit sendData(address,"FL"+moveLength);
        emit sendData(address,"IP");
        emit sendData(address,"AL");
        emit sendData(address,"RS");
    }
}


void movework::delay(int t)
{
    QEventLoop loop;
    QTimer::singleShot(t,&loop,SLOT(quit()));
    loop.exec();
}
