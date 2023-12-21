#ifndef SETUI_H
#define SETUI_H

#include <QWidget>

namespace Ui {
class setui;
}

class setui : public QWidget
{
    Q_OBJECT

public:
    explicit setui(QWidget *parent = nullptr);
    ~setui();
    Ui::setui *ui;

private:

};

#endif // SETUI_H
