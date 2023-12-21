#include "moveui.h"
#include "ui_moveui.h"

moveui::moveui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::moveui)
{
    ui->setupUi(this);
}

moveui::~moveui()
{
    delete ui;
}
