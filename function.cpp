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

void Widget::loadlocalmotor(QString filePath)
{
    rootPath  = QCoreApplication::applicationDirPath();

    QFile file(rootPath + filePath);

    file.open(QIODevice::ReadOnly);
    QByteArray jsonData = file.readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
    QJsonObject jsonObject = jsonDoc.object();


    for (int i = 0;i<8;i++)
    {
        QString jsonindex = QString::number(i);

        mymotor[i].name = jsonObject.value("name"+jsonindex).toString();
        mymotor[i].address = jsonObject.value("address"+jsonindex).toString();
        mymotor[i].type = jsonObject.value("type"+jsonindex).toString();
        mymotor[i].version = jsonObject.value("version"+jsonindex).toString();


        mymotor[i].speed = jsonObject.value("speed"+jsonindex).toString();
        mymotor[i].rateLimit = jsonObject.value("speedLimit"+jsonindex).toString();
        mymotor[i].acc[0] = jsonObject.value("acc"+jsonindex).toArray()[0].toString();
        mymotor[i].acc[1] = jsonObject.value("acc"+jsonindex).toArray()[1].toString();

        mymotor[i].position = jsonObject.value("position"+jsonindex).toString();
        mymotor[i].positionLimit[0] = jsonObject.value("positionLimit"+jsonindex).toArray()[0].toString();
        mymotor[i].positionLimit[1] = jsonObject.value("positionLimit"+jsonindex).toArray()[1].toString();

        mymotor[i].alarm = jsonObject.value("alarm"+jsonindex).toString();
        mymotor[i].zero = jsonObject.value("zero"+jsonindex).toString();
        mymotor[i].resolution = jsonObject.value("resolution"+jsonindex).toString();
        mymotor[i].len = jsonObject.value("len"+jsonindex).toString().toInt();


    }
    for (int i = 0;i<8;i++)
    {
        if (mymotor[i].name == "")
        {
            for (int j = 0;j<7-i;j++)
            {
                mymotor[i]=mymotor[i+1];
            }
            if(i==7)mymotor[i] = *new motor;
        }
        refreshUi(mymotor[i],i);
    }

}


void Widget::setParam(motor myMotor,int index,QString filePath)
{

    setui *nw = new setui();
    nw->setAttribute(Qt::WA_DeleteOnClose);
    nw->show();

    //初始化界面
    nw->ui->nameSet->setText(myMotor.name);
    nw->ui->addressSet->setText(myMotor.address);
    nw->ui->typeSet->setText(myMotor.type);
    nw->ui->versionSet->setText(myMotor.version);

    nw->ui->speedLimitSet->setValue(myMotor.rateLimit.toInt());
    nw->ui->speedSet->setValue(myMotor.speed.toInt());
    nw->ui->accSet->setValue(myMotor.acc[0].toInt());
    nw->ui->decSet->setValue(myMotor.acc[1].toInt());

    nw->ui->positionSet->setValue(myMotor.position.toInt());
    nw->ui->resolution->setValue(myMotor.resolution.toInt());

    nw->ui->ex_pos->setValue(myMotor.position.toDouble()/nw->ui->resolution->value()/myMotor.len);

//    void (QSpinBox::* pQSpinBoxSignal)(int) = &QSpinBox::valueChanged;
//    QObject::connect(nw->ui->positionSet, pQSpinBoxSignal, [=](){
//        double temp = nw->ui->positionSet->text().toDouble()/nw->ui->resolution->value();
//        nw->ui->ex_pos->setValue(temp);
//    });


    void (QDoubleSpinBox::* pQSpinBoxSignal2)(double) = &QDoubleSpinBox::valueChanged;
    QObject::connect(nw->ui->ex_pos, pQSpinBoxSignal2, [=](){
        double temp = nw->ui->ex_pos->value()*nw->ui->resolution->value()*myMotor.len;
        nw->ui->positionSet->setValue(int(temp));
    });

    nw->ui->positionLimitSet1->setValue(myMotor.positionLimit[0].toInt());
    nw->ui->positionLimitSet2->setValue(myMotor.positionLimit[1].toInt());


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

        mymotor[index].zero = myMotor.zero;
        mymotor[index].resolution = nw->ui->resolution->text();

        motorset(mymotor[index]);


        saveJson(mymotor[index],index,filePath);

        refreshUi(mymotor[index],index);

        nw->close();
    });
    QObject::connect(nw->ui->cancel,&QPushButton::clicked,
                     [=]()
    {
        nw->close();
    });

}

void Widget::saveJson(motor myMotor,int index,QString filePath)
{
    QFile file(rootPath+filePath);

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


void Widget::refreshUi(motor Mymotor,int index)
{

    QLineEdit*name = findChild<QLineEdit*>("name_"+QString::number(index+1));
    QLineEdit*position = findChild<QLineEdit*>("position_"+QString::number(index+1));
    QLineEdit*ex_pos = findChild<QLineEdit*>("ex_pos_"+QString::number(index+1));
    QLineEdit*realposition = findChild<QLineEdit*>("realposition_"+QString::number(index+1));
    QLineEdit*ex_real_pos = findChild<QLineEdit*>("ex_real_pos_"+QString::number(index+1));

    findChild<QPushButton*>("pushButton_"+QString::number(index+1))->setText(Mymotor.name);


    name->setText(Mymotor.name);

    position->setText(Mymotor.position);



    ex_pos->setText(QString::number(position->text().toDouble()/mymotor[index].resolution.toInt()/Mymotor.len));

    QObject::connect(position, &QLineEdit::textChanged, [=](){
       ex_pos->setText(QString::number(position->text().toDouble()/mymotor[index].resolution.toInt()/Mymotor.len));
    });


    QObject::connect(realposition, &QLineEdit::textChanged, [=](){
        ex_real_pos ->setText(QString::number(realposition->text().toDouble()/mymotor[index].resolution.toInt()/Mymotor.len));
    });

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
    }
}

void  Widget::reseiveMessage()
{
    delay(40);
    QString ress = myPort->readAll();
    qDebug()<<ress;
    QRegExp newline("\r");
    if(ress.indexOf(newline)!=-1)
    {
        ress = ress.left(ress.indexOf(newline));

    }
    else ress = "9"; //自己设置的零响应代码


    if(0<ress.mid(0,1).toInt() and ress.mid(0,1).toInt()<9)
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
    QString address = res.mid(0,1);

    int motoraddress = NULL;

    QList<QString> motorlist = {};

    for(int i = 0; i<8; i++)
    {
        if (mymotor[i].address!="") motorlist.append(mymotor[i].address);
        if (mymotor[i].address==address)
        {
           findChild<QPushButton*>("pushButton_"+QString::number(i+1))->setText(mymotor[i].name);
           findChild<QPushButton*>("pushButton_"+QString::number(i+1))->setStyleSheet("background-color: yellow");
           refreshUi(mymotor[i],i);
           motoraddress = i;
           onlinecounter[i] = 0;
        }
        onlinecounter[i]++;
        if(onlinecounter[i]>=3)//无应答时间过长，默认电机下线
        {
            findChild<QPushButton*>("pushButton_"+QString::number(i+1))->setStyleSheet("background-color: white");
        }
    }
    int temp = NULL;
    for (int i = 0;i<motorlist.length();i++)
    {
        if(address==motorlist[i]) break;
        temp = i+1;
    }

    if(temp==motorlist.length() and address.toInt()<9 and address!="9")//新电机
    {
        mymotor[temp].address=address;
        mymotor[temp].name="new";
        findChild<QPushButton*>("pushButton_"+QString::number(temp+1))->setText(mymotor[temp].name);
        findChild<QPushButton*>("pushButton_"+QString::number(temp+1))->setStyleSheet("background-color: yellow");

        refreshUi(mymotor[temp],temp);
    }


    if (address!="9")
    {
        if(res.mid(1,2)=="IP")//位置
        {

            QString IP = res.mid(4,-1);
            mymotor[motoraddress].realPosition = IP;
            qDebug()<<IP;
            findChild<QLineEdit*>("realposition_"+QString::number(motoraddress+1))
                    ->setText(QString::number(IP.toInt()-mymotor[motoraddress].zero.toInt()));
        }

        else if(res.mid(1,2)=="AL")//报警
        {

            QString alarm = res.mid(4,-1);

            if(alarm!="0000")
                findChild<QCheckBox*>("alarmflag_"+QString::number(motoraddress+1))->setChecked(true);
            else
                findChild<QCheckBox*>("alarmflag_"+QString::number(motoraddress+1))->setChecked(false);

            switch (alarm.mid(2,1).toInt())
            {
                case 8://过流
                    findChild<QCheckBox*>("overcurrent_"+QString::number(motoraddress+1))->setChecked(true);
                    break;
                default:
                    findChild<QCheckBox*>("overcurrent_"+QString::number(motoraddress+1))->setChecked(false);
                    break;
            }

            switch (alarm.mid(3,1).toInt())
            {
                case 2://反转限位 1 0002
                    findChild<QCheckBox*>("ccwlimit_"+QString::number(motoraddress+1))->setChecked(true);
                    break;
                case 4://正转限位 2 0004
                    findChild<QCheckBox*>("cwlimit_"+QString::number(motoraddress+1))->setChecked(true);
                    break;
                case 8://过温
                    findChild<QCheckBox*>("overvtemp_"+QString::number(motoraddress+1))->setChecked(true);
                    break;
                default:
                    findChild<QCheckBox*>("overvtemp_"+QString::number(motoraddress+1))->setChecked(false);
                    findChild<QCheckBox*>("cwlimit_"+QString::number(motoraddress+1))->setChecked(false);
                    findChild<QCheckBox*>("ccwlimit_"+QString::number(motoraddress+1))->setChecked(false);
                    break;
            }

        }
        else if(res.mid(1,2)=="RS")//状态
        {
            QString state = res.mid(4,-1);

            if (state.contains("F") or state.contains("J") or state.contains("M"))//    A=报警出现 D=驱动器禁止E=驱动器出错H=正在找原点J=正在点动F=正在运动（Feed&Jog 指令）M=正在运动（任意输入信号）R=准备完毕
            {
                findChild<QCheckBox*>("moveflag_"+QString::number(motoraddress+1))->setChecked(true);
                findChild<QCheckBox*>("OKflag_"+QString::number(motoraddress+1))->setChecked(false);
            }
            else
            {
                findChild<QCheckBox*>("moveflag_"+QString::number(motoraddress+1))->setChecked(false);
                findChild<QCheckBox*>("OKflag_"+QString::number(motoraddress+1))->setChecked(true);
            }

        }

    }

}


void Widget::closeEvent(QCloseEvent *e)
{
    this->setEnabled(false);
    QMessageBox msgBox;
    msgBox.setWindowTitle("提示");
    msgBox.setText("\n确 认 退 出 ?\n\n点击确认会安全保留您所作的更改\n");
    msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    if(ret == QMessageBox::Ok)
    {
        //窗口关闭
        ui->statebox->append("-------------------------");
        ui->statebox->append("正在保存日志");
        for (int i = 0;i<8;i++)
        {
            saveJson(mymotor[i],i,filepath);
        }
        sendlist.clear();
        myPort->close();
        delay(1000);

        ui->statebox->append("-------------------------");
        ui->statebox->append("安全退出");
        delay(1500);

        e->accept();
    }
    else
    {
        //忽略
        this->setEnabled(true);
        ui->statebox->append("取消退出");
        e->ignore();
    }

}
