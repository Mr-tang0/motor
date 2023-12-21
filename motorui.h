#ifndef MOTORUI_H
#define MOTORUI_H

#include <QWidget>

namespace Ui {
class motorui;
}

class motorui : public QWidget
{
    Q_OBJECT

public:
    explicit motorui(QWidget *parent = nullptr);
    ~motorui();

private:
    Ui::motorui *ui;
};

#endif // MOTORUI_H
