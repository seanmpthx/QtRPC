#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "rcpx.h"
#include "QtRPCManager.h"

namespace Ui {
class Widget;
}


class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);

    ~Widget();

    void closeEvent(QCloseEvent *e);
    void appendText(const QString &text);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    QString temp();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

private:
    Ui::Widget *ui;
    QByteArray dataImage;
    MyRPC *myRpc;
};

#endif // WIDGET_H
