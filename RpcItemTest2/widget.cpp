#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

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
    widget = this;

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
    qInstallMessageHandler(MsgHandlerCallback);
#else
    qInstallMsgHandler(MsgHandlerCallback);
#endif

    myRpc = QtRPCManager::instance().reg<MyRPC>(QtRPCItem::SocketTcp, QtRPCItem::ConnectionPeer);

    myRpc->registerSlot(this, "TestFunction1", rpc_functoin_testfunction1);
    myRpc->registerSlot(this, "testFunction2", rpc_functoin_testfunction2);
    myRpc->registerSlot(this, "testFunction6", rpc_functoin_testfunction6);
    myRpc->registerSlot(this, "funcTest7", rpc_functoin_testfunction7);
    myRpc->registerSlot(this, "testFunction8", rpc_functoin_testfunction8);
    myRpc->registerSlot(this, "testFunction9", rpc_functoin_testfunction9);
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

QString Widget::testFunction2()
{
    qDebug() << "call test2";
    return "adfs";
}

void Widget::TestFunction1()
{
    qDebug() << "call test1";
}

QMap<QString, QPair<QString, QString> > Widget::testFunction6(const QString &conf)
{
    QMap<QString, QPair<QString, QString> > temp;
    if (conf == "1")
    {
        temp.insert("1", QPair<QString,QString>("2", "3"));
        return temp;
    }
    temp.insert("4", QPair<QString,QString>("5", "6"));
    return temp;
}

#include <QFile>
#include <QFileInfo>
void Widget::funcTest7(const QByteArray &data)
{
    static int index = 0;
    QFile file(QString::number(index++) + ".png");
    file.open(QIODevice::WriteOnly);
    file.write(data);
    file.close();
    qDebug() << "save image:" << QFileInfo(file.fileName()).absoluteFilePath();
}

void Widget::testFunction8(const DataTest1 &data)
{
    qDebug() << "testFunction8, DataTest1.arg1" << data.arg1 << " arg2:" << data.arg2 << " arg3:" << data.arg3;
}

DataTest2 Widget::testFunction9()
{
    DataTest2 data2;
    data2.arg1 << "1" << "2" << "3" << "5" << "6" << "888888";
    return data2;
}

