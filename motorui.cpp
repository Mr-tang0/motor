#include "motorui.h"
#include "ui_motorui.h"

motorui::motorui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::motorui)
{
    ui->setupUi(this);
}

motorui::~motorui()
{
    delete ui;
}
