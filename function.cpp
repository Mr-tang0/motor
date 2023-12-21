#include "widget.h"
#include "ui_widget.h"


#include <QFileInfo>
#include <QDir>

#include "movework.h"


void Widget::findPort()
{
    ui->portBox->clear();
    QList<QSerialPortInfo> serialPorts = QSerialPortInfo::availablePorts();
    foreach(const QSerialPortInfo &serialPortInfo, serialPorts)
    {
        ui->portBox->addItem(serialPortInfo.portName());
    }
}


void Widget::setParam(motor myMotor,int index)
{
    setui *nw = new setui();
    nw->setAttribute(Qt::WA_DeleteOnClose);
    nw->show();

    QFileInfo fileinfo(QDir::currentPath());
    rootPath  = fileinfo.path();
    QFile file(rootPath+"/demo/temp.json");
    file.open(QIODevice::ReadOnly);
    QByteArray jsonData = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject jsonObject = jsonDoc.object();

    QString i = QString::number(index);

    //初始化界面
    nw->ui->nameSet->setText(jsonObject.value("name"+i).toString());
    nw->ui->addressSet->setText(jsonObject.value("address"+i).toString());
    nw->ui->typeSet->setText(jsonObject.value("type"+i).toString());
    nw->ui->versionSet->setText(jsonObject.value("version"+i).toString());

    nw->ui->speedLimitSet->setValue(jsonObject.value("rateLimit"+i).toString().toInt());
    nw->ui->speedSet->setValue(jsonObject.value("speed"+i).toString().toInt());

    nw->ui->accSet->setValue(jsonObject.value("acc"+i).toArray()[0].toString().toInt());
    nw->ui->decSet->setValue(jsonObject.value("acc"+i).toArray()[1].toString().toInt());

    nw->ui->positionSet->setValue(jsonObject.value("position"+i).toString().toInt());

    nw->ui->resolution->setValue(jsonObject.value("resolution"+i).toString().toInt());


    nw->ui->ex_pos->setValue(jsonObject.value("position"+i).toString().toDouble()/nw->ui->resolution->value());


    void (QSpinBox::* pQSpinBoxSignal)(int) = &QSpinBox::valueChanged;
    QObject::connect(nw->ui->positionSet, pQSpinBoxSignal, [=](){
        double temp = nw->ui->positionSet->text().toDouble()/nw->ui->resolution->value();
        nw->ui->ex_pos->setValue(temp);
    });


    void (QDoubleSpinBox::* pQSpinBoxSignal2)(double) = &QDoubleSpinBox::valueChanged;
    QObject::connect(nw->ui->ex_pos, pQSpinBoxSignal2, [=](){
        double temp = nw->ui->ex_pos->value()*nw->ui->resolution->value();
        nw->ui->positionSet->setValue(int(temp));
    });

    nw->ui->positionLimitSet1->setValue(jsonObject.value("positionLimit"+i).toArray()[0].toString().toInt());
    nw->ui->positionLimitSet2->setValue(jsonObject.value("positionLimit"+i).toArray()[1].toString().toInt());

    file.close();

    //保存更改
    QObject::connect(nw->ui->yes,&QPushButton::clicked,
                     [=]()
    {        

        mymotor[index].name = nw->ui->nameSet->text();
        mymotor[index].address = nw->ui->addressSet->text();
        mymotor[index].type = nw->ui->typeSet->text();
        mymotor[index].version = nw->ui->versionSet->text();

        mymotor[index].rateLimit = nw->ui->speedLimitSet->text();
        mymotor[index].speed = nw->ui->speedSet->text();
        mymotor[index].acc[0] = nw->ui->accSet->text();
        mymotor[index].acc[1] = nw->ui->decSet->text();
        mymotor[index].position = nw->ui->positionSet->text();
        mymotor[index].positionLimit[0] = nw->ui->positionLimitSet1->text();
        mymotor[index].positionLimit[1] = nw->ui->positionLimitSet2->text();

        mymotor[index].zero = jsonObject.value("zero"+i).toString();
        mymotor[index].resolution = nw->ui->resolution->text();

        motorset(mymotor[index]);
        saveJson(mymotor[index],index);
        refreshUi();
        nw->close();
    });
    QObject::connect(nw->ui->cancel,&QPushButton::clicked,
                     [=]()
    {
        nw->close();
    });

}

void Widget::saveJson(motor myMotor,int index)
{
    QFile file(rootPath+"/demo/temp.json");

    file.open(QIODevice::ReadOnly);
    QByteArray jsonData = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject myObject = jsonDoc.object();
    file.close();

    QString i  = QString::number(index);

    myObject["name"+i] = myMotor.name;
    myObject["address"+i] = myMotor.address;
    myObject["type"+i] = myMotor.type;
    myObject["version"+i] = myMotor.version;
    myObject["rateLimit"+i] = myMotor.rateLimit;
    myObject["speed"+i] = myMotor.speed;

    QJsonArray arr0 = {myMotor.acc[0],myMotor.acc[1]};
    myObject["acc"+i] = arr0;

    myObject["position"+i] = myMotor.position;

    QJsonArray arr1 = {myMotor.positionLimit[0],myMotor.positionLimit[1]};
    myObject["positionLimit"+i] = arr1;

    myObject["zero"+i] = myMotor.zero;
    myObject["alarm"+i] = myMotor.alarm;
    myObject["resolution"+i] = myMotor.resolution;

    jsonDoc.setObject(myObject);

    QString temp  = jsonDoc.toJson();

    file.open(QIODevice::WriteOnly);
    file.write(temp.toUtf8());
    file.close();
    ui->statebox->append("保存成功");

}


void Widget::refreshUi()
{

    QList<QLineEdit*> names = {ui->name_1,ui->name_2,ui->name_3,ui->name_4,
                               ui->name_5,ui->name_6,ui->name_7,ui->name_8};
    QList<QLineEdit*> position = {ui->position_1,ui->position_2,ui->position_3,ui->position_4,
                                  ui->position_5,ui->position_6,ui->position_7,ui->position_8};

    QList<QLineEdit*> exposition = {ui->ex_pos_1,ui->ex_pos_2,ui->ex_pos_3,ui->ex_pos_4,
                                  ui->ex_pos_5,ui->ex_pos_6,ui->ex_pos_7,ui->ex_pos_8};

    QList<QLineEdit*> realposition = {ui->realposition_1,ui->realposition_2,ui->realposition_3,ui->realposition_4,
                                  ui->realposition_5,ui->realposition_6,ui->realposition_7,ui->realposition_8};

    QList<QLineEdit*> ex_realposition = {ui->ex_real_pos_1,ui->ex_real_pos_2,ui->ex_real_pos_3,ui->ex_real_pos_4,
                                  ui->ex_real_pos_5,ui->ex_real_pos_6,ui->ex_real_pos_7,ui->ex_real_pos_8};


    for (int i = 0; i<8; i++)
    {

        names[i]->setText(mymotor[i].name);

        position[i]->setText(mymotor[i].position);

        QObject::connect(position[i], &QLineEdit::textChanged, [=](){
            exposition[i]->setText(QString::number(position[i]->text().toDouble()/mymotor[i].resolution.toInt()));
        });
        QObject::connect(realposition[i], &QLineEdit::textChanged, [=](){
            ex_realposition[i]->setText(QString::number(realposition[i]->text().toDouble()/mymotor[i].resolution.toInt()));
        });


    }


}




void Widget::connectPort()
{
    portName = ui->portBox->currentText();

    myPort->setPortName(portName);
    switch (ui->baundBox->currentIndex())
    {
        case 0:myPort->setBaudRate(QSerialPort::Baud4800);break;
        case 1:myPort->setBaudRate(QSerialPort::Baud9600);break;
        default:myPort->setBaudRate(QSerialPort::Baud9600);break;
    }

    openFlag = myPort->open(QIODevice::ReadWrite);

    if(openFlag)
    {
        ui->statebox->append("开启成功");
        ui->openfg->setChecked(true);
    }
    else
    {
        ui->statebox->append("开启失败");
        ui->openfg->setChecked(false);
    }
}

void Widget::appendMessage(const int &index,const QString &data)
{   
    QString id = QString::number(index);
    if(openFlag)
    {
        QByteArray temp = QByteArray((id+data).toUtf8());
        temp.append("\r");
        sendlist.append(temp);
        qDebug()<<sendlist;

    }
}
void Widget::prependMessage(const int &index,const QString &data)
{
    QString id = QString::number(index);
    if(openFlag)
    {
        QByteArray temp = QByteArray((id+data).toUtf8());
        temp.append("\r");
        sendlist.prepend(temp);
        qDebug()<<sendlist;
    }
}

void  Widget::reseiveMessage()
{
    QString ress = myPort->readAll();
    delay(50);
    qDebug()<<ress;
    QRegExp newline("\r");
    if(ress.indexOf(newline)!=-1)
    {
        ress = ress.left(ress.indexOf(newline));
    }
    else ress = "9"; //自己设置的零响应代码

    decode(ress);

}

void Widget::motorset(motor myMotor)
{

    int address = myMotor.address.toInt();
    prependMessage(address,"EG"+myMotor.resolution);//分辨率

    prependMessage(address,"AC"+myMotor.acc[0]);//加速度
    prependMessage(address,"DE"+myMotor.acc[1]);//减速度

    if(myMotor.speed.toInt()>myMotor.rateLimit.toInt())//软件速度限制
    {
        prependMessage(address,"VE"+myMotor.rateLimit);
    }
    else prependMessage(address,"VE"+myMotor.speed);

}

void Widget::motorMove(motor myMotor,bool d)
{
    stopflag = 0;
    threadMotor = &myMotor;
    QThread *t = new QThread();

    movework *work = new movework;
    work->moveToThread(t);

    if(d)
    {
        QObject::connect(work,&movework::sendData,this,[=](int address,QString data)
        {
            prependMessage(address,data);
        });
    }
    else
    {
        QObject::connect(work,&movework::sendData,this,[=](int address,QString data)
        {
            if(data.mid(0,2)=="FL")
            {
                data.insert(2,"-");
            }
            prependMessage(address,data);
        });
    }
    QObject::connect(t,&QThread::finished,t,&QObject::deleteLater);

    t->start();
    work->motorMove();

}

void Widget::moveTo(motor myMotor,int len)
{
    QString x = QString::number(len+myMotor.zero.toInt()-myMotor.realPosition.toInt());

    int i = len+myMotor.zero.toInt()-myMotor.realPosition.toInt();
    myMotor.temp = abs(i);
    if (i>0)motorMove(myMotor,true);
    else motorMove(myMotor,false);

    ui->statebox->append("已发送目标位置");

}

void Widget::delay(int t)
{
    QEventLoop loop;
    QTimer::singleShot(t,&loop,SLOT(quit()));
    loop.exec();
}

void Widget::decode(QString res)
{
    QString index = res.mid(0,1);

    int motorindex;
    for(int i = 0; i<8; i++)
    {
        if (mymotor[i].address.toInt()==index.toInt())
        {
            motorindex = i;
            findChild<QPushButton*>("pushButton_"+QString::number(motorindex+1))->setText(mymotor[motorindex].name);
        }
    }

    if (index!="9")
    {
        if(res.mid(1,2)=="IP")//位置
        {
            QString IP = res.mid(4,-1);

            findChild<QLineEdit*>("realposition_"+QString::number(motorindex+1))
                    ->setText(QString::number(IP.toInt()-mymotor[motorindex].zero.toInt()));
            mymotor[motorindex].realPosition = IP;
        }

        else if(res.mid(1,2)=="AL")//报警
        {

            QString alarm = res.mid(4,-1);

            if(alarm!="0000")
                findChild<QCheckBox*>("alarmflag_"+QString::number(motorindex+1))->setChecked(true);
            else
                findChild<QCheckBox*>("alarmflag_"+QString::number(motorindex+1))->setChecked(false);

            switch (alarm.mid(2,1).toInt())
            {
                case 8://过流
                    findChild<QCheckBox*>("overcurrent_"+QString::number(motorindex+1))->setChecked(true);
                    break;
                default:
                    findChild<QCheckBox*>("overcurrent_"+QString::number(motorindex+1))->setChecked(false);
                    break;
            }

            switch (alarm.mid(3,1).toInt())
            {
                case 2://反转限位 1 0002
                    findChild<QCheckBox*>("ccwlimit_"+QString::number(motorindex+1))->setChecked(true);
                    break;
                case 4://正转限位 2 0004
                    findChild<QCheckBox*>("cwlimit_"+QString::number(motorindex+1))->setChecked(true);
                    break;
                case 8://过温
                    findChild<QCheckBox*>("overvtemp_"+QString::number(motorindex+1))->setChecked(true);
                    break;
                default:
                    findChild<QCheckBox*>("overvtemp_"+QString::number(motorindex+1))->setChecked(false);
                    findChild<QCheckBox*>("cwlimit_"+QString::number(motorindex+1))->setChecked(false);
                    findChild<QCheckBox*>("ccwlimit_"+QString::number(motorindex+1))->setChecked(false);
                    break;
            }

        }
        else if(res.mid(1,2)=="RS")//状态
        {
            QString state = res.mid(4,-1);

            if (state.contains("F") or state.contains("J") or state.contains("M"))//    A=报警出现 D=驱动器禁止E=驱动器出错H=正在找原点J=正在点动F=正在运动（Feed&Jog 指令）M=正在运动（任意输入信号）R=准备完毕
            {
                findChild<QCheckBox*>("moveflag_"+QString::number(motorindex+1))->setChecked(true);
                findChild<QCheckBox*>("OKflag_"+QString::number(motorindex+1))->setChecked(false);
            }
            else
            {
                findChild<QCheckBox*>("moveflag_"+QString::number(motorindex+1))->setChecked(false);
                findChild<QCheckBox*>("OKflag_"+QString::number(motorindex+1))->setChecked(true);
            }

        }

    }
}