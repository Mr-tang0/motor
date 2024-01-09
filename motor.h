#ifndef MOTOR_H
#define MOTOR_H
#include<QString>
#include<QFile>

class motor
{

public:
    motor();

    QString name,address,type,version;//基础信息
    QString rateLimit,speed,acc[2],position,positionLimit[2];//配置信息
    QString zero,alarm;//原点和报警
    QString resolution,resolution2;
    QString realPosition;
    int len;
    int temp;

};

#endif // MOTOR_H
