#include "widget.h"
#include "ui_widget.h"
#include <QMetaMethod>
#include <QDebug>
#include <QEventLoop>
#include <QTimer>

static Widget *widget = 0;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
void MsgHandlerCallback(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    widget->appendText(msg);
}
#else
void MsgHandlerCallback(QtMsgType type, const char *msg)
{
    widget->appendText(msg);
}
#endif

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    myRpc = qRpc.reg<MyRPC>(QtRPCItem::SocketTcp, QtRPCItem::ConnectionPeer);

    widget = this;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    qInstallMessageHandler(MsgHandlerCallback);
#else
    qInstallMsgHandler(MsgHandlerCallback);
#endif

}

Widget::~Widget()
{
    delete ui;
}

void Widget::closeEvent(QCloseEvent *e)
{
    QtRPCManager::instance().releaseBeforeEventLoopExit();
    QWidget::closeEvent(e);
}

void Widget::appendText(const QString &text)
{
    ui->plainTextEdit->appendPlainText(text);
}

void Widget::on_pushButton_clicked()
{
    QString temp = myRpc->testFunction2();
    qDebug() << temp;
}

void Widget::on_pushButton_2_clicked()
{
    QMap < QString , QPair < QString , QString > >  temp = myRpc->testFunction6("1");
    qDebug() << temp;
}

void Widget::on_pushButton_3_clicked()
{
    QMap < QString , QPair < QString , QString > >  temp = myRpc->testFunction6("2");
    qDebug() << temp;
}

#include <QFile>
void Widget::on_pushButton_4_clicked()
{
    if (dataImage.isEmpty())
    {
        QFile file(":/1.png");
        file.open(QIODevice::ReadOnly);
        dataImage = file.readAll();
        file.close();
    }
    myRpc->testFunction7(dataImage);
}

QString Widget::temp()
{
    return "QString111";
}

void Widget::on_pushButton_5_clicked()
{
    DataTest1 data1;
    data1.arg1 = "arg1";
    data1.arg2 = 76543;
    data1.arg3 = 101;
    myRpc->testFunction8(data1);
}

void Widget::on_pushButton_6_clicked()
{
    DataTest2 data2 = myRpc->testFunction9();
    qDebug() << "DataTest2.arg1:" << data2.arg1;
}
