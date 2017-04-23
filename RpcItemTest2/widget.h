#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "QtRPCManager.h"
#include "rcpx_rpc_callbacks.h"
#include <QUrl>

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

public slots:
    QString testFunction2();
    void TestFunction1();
    QMap<QString, QPair<QString, QString> > testFunction6(const QString &conf);
    void funcTest7(const QByteArray &data);

    void testFunction8(const DataTest1 &data);
    DataTest2 testFunction9();

private:
    Ui::Widget *ui;
    MyRPC *myRpc;
};

#endif // WIDGET_H
