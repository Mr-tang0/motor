#include "setui.h"
#include "ui_setui.h"

setui::setui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::setui)
{
    ui->setupUi(this);
}

setui::~setui()
{
    delete ui;
}
