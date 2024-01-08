#include "widget.h"
#include "ui_widget.h"



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
    initUi(0.6);
    findPort();

    filepath = "/temp.json";
    imgpath = ":/new/prefix1/img/uibright.png";
    appversion = "Version:1.2.20230108";
    ui->label_Version->setText(appversion);
    loadlocalmotor(filepath);


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

        QPushButton *clearbtn = findChild<QPushButton*>("clear_"+QString::number(i+1));
        QPushButton *deletebtn = findChild<QPushButton*>("delete_"+QString::number(i+1));

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
            prependMessage(mymotor[i].address.toInt(),"SK");//停止
        });

        QObject::connect(enablebtn,&QPushButton::clicked,[=]()
        {
            prependMessage(mymotor[i].address.toInt(),"ME");//使能
            findChild<QCheckBox*>("enableflag_"+QString::number(i+1))->setChecked(true);
        });

        QObject::connect(disablebtn,&QPushButton::clicked,[=]()
        {
            prependMessage(mymotor[i].address.toInt(),"MD");//非使能
            findChild<QCheckBox*>("enableflag_"+QString::number(i+1))->setChecked(false);
        });
        QObject::connect(setbtn,&QPushButton::clicked,[=]()
        {
            setParam(mymotor[i],i,filepath);//设置
        });
        QObject::connect(set0btn,&QPushButton::clicked,[=]()  //运动时禁止使用
        {
            mymotor[i].zero = mymotor[i].realPosition;
            saveJson(mymotor[i],i,filepath);
        });
        QObject::connect(clearbtn,&QPushButton::clicked,[=]()
        {
            prependMessage(mymotor[i].address.toInt(),"AR");//清除报警
        });
        QObject::connect(deletebtn,&QPushButton::clicked,[=]()
        {
            for (int j = i;j<7;j++)
            {
                mymotor[j]=mymotor[j+1];
                refreshUi(mymotor[j],j);
            }
            if(i==7)
            {
                mymotor[i]=*new motor;
                refreshUi(mymotor[i],i);
            }
        });
    }


}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_refresh_clicked()
{
    findFlag = true;
    findPort();

}

void Widget::on_connect_clicked()
{
    connectPort();
    findFlag = true;

    QObject::connect(myPort,&QSerialPort::readyRead,[=]()
    {
        reseiveMessage();
    });

    for(int i =1;i<9;i++)
    {
        QObject::connect(timer,&QTimer::timeout,[=]()
        {
            appendMessage(i,"IP");
            appendMessage(i,"AL");//警报
            appendMessage(i,"RS");//警报
        });
        timer->start(250);


    }

    QObject::connect(timer3,&QTimer::timeout,[=]()
    {            
        int w_x = this->frameGeometry().x();
        int w_y = this->frameGeometry().y();

        QPoint coursePoint = QCursor::pos();
        int p_x = coursePoint.x();
        int p_y = coursePoint.y();

        p_x = p_x - w_x;
        p_y = p_y - w_y;

        QRect rect_state = ui->frame_state->frameGeometry();
        QRect rect_move1 = ui->frame_move1->frameGeometry();
        QRect rect_send = ui->frame_send->frameGeometry();
        if(p_x<rect_state.left() or p_x>rect_move1.right() or p_y<rect_state.top() or p_y>rect_state.bottom())
        {
            if(p_x<rect_send.left() or p_x>rect_send.right() or p_y<rect_send.top() or p_y>rect_send.bottom())
            {
                stopflag = 1;
                sendlist.clear();
                qDebug()<<p_y;
            }
        }

        if(!sendlist.isEmpty())
        {
            myPort->write(sendlist.first());
            sendlist.removeFirst();
        }

    });
    timer3->start(250);
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

    moveTo(mymotor[0],ui->axisx->text().toInt()*mymotor[0].len*mymotor[0].resolution.toInt());
    moveTo(mymotor[1],ui->axisy->text().toInt()*mymotor[0].len*mymotor[0].resolution.toInt());
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

void Widget::on_BlackandWhite_clicked()
{
    if(blackflag)
    {
        imgpath = ":/new/prefix1/img/uibright.png";
        blackflag = false;
        this->resize(this->size()/1.001);//触发背景适应
    }
    else
    {
        imgpath =":/new/prefix1/img/uiDark.png";
        blackflag = true;
        this->resize(this->size()*1.001);//触发背景适应
    }

}

void Widget::on_Clear_clicked()
{
    ui->statebox->clear();
}

void Widget::on_Send_clicked()
{
    QString text = ui->sendline->text();
    if (text!="")
    {
        prependMessage(text.mid(0,1).toInt(),text.mid(1,-1));
        ui->sendline->clear();
        ui->statebox->append(sendlist.first());
    }
    else
        ui->statebox->append("请输入指令\n（格式：地址+指令，如：1AC）");

}

void Widget::on_Style_clicked()
{
    QString qss;

    if(styleflag)
    {
        QFile qssFile(":/new/prefix1/qss/Utbtu.qss");
        qssFile.open(QFile::ReadOnly);
        if(qssFile.isOpen())
        {
          qss = QLatin1String(qssFile.readAll());
          qApp->setStyleSheet(qss);
          qssFile.close();
        }
        styleflag = false;
        this->resize(this->size()/1.001);//触发背景适应
    }
    else
    {
        QFile qssFile(":/new/prefix1/qss/MacOs.qss");
        qssFile.open(QFile::ReadOnly);
        if(qssFile.isOpen())
        {
          qss = QLatin1String(qssFile.readAll());
          qApp->setStyleSheet(qss);
          qssFile.close();
        }
        styleflag = true;
        this->resize(this->size()*1.001);//触发背景适应
    }



}

void Widget::on_About_clicked()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("About");
    QString txt = "                         \n";
    msgBox.setText(txt+appversion+txt+"Powered By Tang\n");
    msgBox.exec();
}

void Widget::on_Help_clicked()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Help");
    msgBox.setText("QitHub:\n"
                   "https://github.com/Mr-tang0/motor\n\n"
                   "Email:\n"
                   "None\n");
    msgBox.exec();
}
