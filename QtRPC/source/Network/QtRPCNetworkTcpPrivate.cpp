#include "QtRPCNetworkTcp.h"
#include "QtRPCNetworkTcpPrivate.h"
#include "../../include/QtRPCManager.h"
#include <QNetworkInterface>
#include <QMetaMethod>

QtRPCNetworkTcpPrivate::QtRPCNetworkTcpPrivate(QtRPCNetworkTcp *q, int maxPeers) :
    q_ptr(q), tcpSocketIn_(NULL), sectionCtrl_(NULL)
{
    connect(&searcher_, SIGNAL(doGotPeer(QString,QString,quint16)),
            this, SLOT(onGotPeer(QString,QString,quint16)));

    createServer();
    searcher_.startSearch("0.0.0.0", myPort_);

}

QtRPCNetworkTcpPrivate::~QtRPCNetworkTcpPrivate()
{
    clearConnections();
}


void QtRPCNetworkTcpPrivate::writeMsg(int msgid, const QByteArray &data)
{
    if (tcpSocketIn_ != NULL)
    {
        qDebug() << "write msg 1";
        writeSections(msgid, data);
        return;
    }

    if (tcpSocketOut_)
    {
        qDebug() << "write msg 2";
        writeSections(msgid, data);
        return;
    }

    qDebug() << "not connected";

}

void QtRPCNetworkTcpPrivate::newConnection()
{
    qDebug() << "new connection.1";
    tcpSocketIn_ = tcpServer_->nextPendingConnection();
    tcpSocketIn_->setSocketOption(QAbstractSocket::LowDelayOption, true);
    connectSignals(tcpSocketIn_);
    tcpServer_->close();
    searcher_.stopSearch();
}

void QtRPCNetworkTcpPrivate::readReady()
{
    if (sectionCtrl_ == NULL)
        return;
    qDebug() << "read ready";
    sectionCtrl_->read();
}

void QtRPCNetworkTcpPrivate::connected()
{
    qDebug() << "connected";
    searcher_.stopSearch();
}

void QtRPCNetworkTcpPrivate::disconnected()
{
    qDebug() << "disconnected" << this->sender();
    QTimer::singleShot(0, this, SLOT(recreateConnection()));
}

void QtRPCNetworkTcpPrivate::error(QAbstractSocket::SocketError socketError)
{
    qDebug() << "local socket error:" << socketError;
}

void QtRPCNetworkTcpPrivate::createClient()
{
    tcpSocketOut_ = QSharedPointer<QTcpSocket>(new QTcpSocket());
    QObject *obj = tcpSocketOut_.data();
    connectSignals(obj);
}

void QtRPCNetworkTcpPrivate::createServer()
{
    tcpServer_ = QSharedPointer<QTcpServer>(new QTcpServer());
    tcpServer_->setMaxPendingConnections(1);

    for (int i = 8013; i < 9000; ++i)
    {
        if (!tcpServer_->listen(QHostAddress::Any, i))
            continue;

        myPort_ = i;
        break;
    }

    qDebug() << "create local server at:" << myPort_ << " " << this;

    connect(tcpServer_.data(), SIGNAL(newConnection()),
            this, SLOT(newConnection()));
}

void QtRPCNetworkTcpPrivate::clearConnections()
{
    if (sectionCtrl_ != 0)
    {
        delete sectionCtrl_;
        sectionCtrl_ = 0;
    }

    if (tcpSocketIn_ != 0)
    {
        tcpSocketIn_->blockSignals(true);
        tcpSocketIn_->close();
        delete tcpSocketIn_;
        tcpSocketIn_ = 0;
    }

    if (tcpSocketOut_)
    {
        tcpSocketOut_->blockSignals(true);
        tcpSocketOut_->close();
        tcpSocketOut_.clear();
    }

    if (tcpServer_)
    {
        tcpServer_->blockSignals(true);
        tcpServer_->close();
        tcpServer_.clear();
    }

    searcher_.stopSearch();
}


void QtRPCNetworkTcpPrivate::recreateConnection()
{
    this->clearConnections();


    createServer();
    searcher_.startSearch("0.0.0.0", myPort_);
}

void QtRPCNetworkTcpPrivate::onGotPeer(const QString &/*address*/, const QString &ip, quint16 port)
{
    qDebug() << "got peer," << "ip:" << ip << ":" << port;
    connectToServer(ip, port);
}

void QtRPCNetworkTcpPrivate::connectToServer(const QString &address, quint16 port)
{
    if (tcpServer_)
        tcpServer_->close();

    createClient();
    tcpSocketOut_->setSocketOption(QAbstractSocket::LowDelayOption, true);
    tcpSocketOut_->connectToHost(address, port);
}

void QtRPCNetworkTcpPrivate::connectSignals(QObject *obj)
{
    if (sectionCtrl_ == 0)
    {
        sectionCtrl_ = new MsgSection((QIODevice*)obj);
        connect(sectionCtrl_, SIGNAL(doGotMsg(int,QByteArray)),
                q_ptr, SIGNAL(doGotMsg(int,QByteArray)));
    }

    connect(obj, SIGNAL(connected()),
            this, SLOT(connected()));
    connect(obj, SIGNAL(readyRead()),
            this, SLOT(readReady()));
    connect(obj, SIGNAL(disconnected()),
            this, SLOT(disconnected()));
    connect(obj, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));
}

void QtRPCNetworkTcpPrivate::writeSections(int id, const QByteArray &data)
{
    if (sectionCtrl_ == 0)
        return;

    sectionCtrl_->write(id, data);
}


