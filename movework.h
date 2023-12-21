#ifndef MOVEWORK_H
#define MOVEWORK_H

#include <QObject>
#include"widget.h"

class movework : public QObject
{
    Q_OBJECT
public:
    explicit movework(QObject *parent = nullptr);

    void delay(int t);
    void motorMove();

signals:
    void sendData(int index,QString data);
    void freshIp(int index,QString data);

public slots:
};

#endif // MOVEWORK_H
