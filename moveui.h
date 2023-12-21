#ifndef MOVEUI_H
#define MOVEUI_H

#include <QWidget>
#include <QtCharts/QValueAxis>

namespace Ui {
class moveui;
}

class moveui : public QWidget
{
    Q_OBJECT

public:
    explicit moveui(QWidget *parent = nullptr);
    ~moveui();

private:
    Ui::moveui *ui;
};

#endif // MOVEUI_H
