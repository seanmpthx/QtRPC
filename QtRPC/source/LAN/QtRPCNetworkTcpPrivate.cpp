#include "QtRPCNetworkTcp.h"
#include "QtRPCNetworkTcpPrivate.h"
#include "../../include/QtRPCManager.h"
#include <QNetworkInterface>
#include <QMetaMethod>
#include <QEventLoop>

QtRPCNetworkTcpPrivate::QtRPCNetworkTcpPrivate(QtRPCNetworkTcp *q, const QString &identifier, int ctype) :
    q_ptr(q), identifier_(identifier), searcher_(identifier, ctype),
    connectionType(ctype), exitMark(false)
{
    connect(&searcher_, SIGNAL(doGotPeer(QString,QString,quint16)),
            this, SLOT(onGotPeer(QString,QString,quint16)));

    createServer();
    searcher_.startSearch("0.0.0.0", myPort_);

    qRegisterMetaType<QTcpSocket*>("QTcpSocket*");

}

QtRPCNetworkTcpPrivate::~QtRPCNetworkTcpPrivate()
{
    qDebug() << "~QtRPCNetworkTcpPrivate";
    exitMark = true;
    clearConnections();
}

void QtRPCNetworkTcpPrivate::writeMsg(int msgid, const QByteArray &data, int socketId)
{
    writeSections(msgid, data, socketId);
}

void QtRPCNetworkTcpPrivate::newConnection()
{
    ++currentPaddings;
    qDebug() << "new connection.1";
    QTcpSocket *tcpSocketIn = tcpServer_->nextPendingConnection();
    tcpSocketIn->setSocketOption(QAbstractSocket::LowDelayOption, true);
    connectSignals(tcpSocketIn);

    searcher_.stopSearch();

    switch(connectionType)
    {
    case QtRPCItem::ConnectionPeer:
    {
        tcpServer_->close();
        break;
    }
    case QtRPCItem::ConnectionRoot:
    {
        break;
    }
    case QtRPCItem::ConnectionBransh:
    {
        tcpServer_->close();
        break;
    }
    case QtRPCItem::ConnectionDistribute:
    {
        if (currentPaddings >= 3)
            searcher_.closeSearch();
        break;
    }
    }
}

void QtRPCNetworkTcpPrivate::readReady()
{
    QTcpSocket *socket = (QTcpSocket*)this->sender();
    QMap<QTcpSocket*, MsgSection*>::iterator itr = sectionCtrls_.find(socket);
    if (itr == sectionCtrls_.end())
        return;
    //qDebug() << "read ready";
    itr.value()->read();
}

void QtRPCNetworkTcpPrivate::connected()
{
    if (tcpServer_)
        tcpServer_->close();

    qDebug() << "connected";
    searcher_.stopSearch();
}

void QtRPCNetworkTcpPrivate::disconnected()
{
    if (currentPaddings > 0)
        --currentPaddings;

    QTcpSocket *socket = (QTcpSocket*)this->sender();
    //qDebug() << "disconnected" << this->sender();
    QMetaObject::invokeMethod(this, "recreateConnection", Qt::QueuedConnection, Q_ARG(QTcpSocket *, socket));
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
    if (connectionType == QtRPCItem::ConnectionBransh)
    {
        return;
    }

    tcpServer_ = QSharedPointer<QTcpServer>(new QTcpServer());
    for (int i = 8013; i < 9000; ++i)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        if (!tcpServer_->listen(QHostAddress::AnyIPv4, i))
            continue;
#else
        if (!tcpServer_->listen(QHostAddress::Any, i))
            continue;
#endif

        myPort_ = i;
        break;
    }

    //qDebug() << "create local server at:" << myPort_ << " " << this;

    connect(tcpServer_.data(), SIGNAL(newConnection()),
            this, SLOT(newConnection()));

    switch(connectionType)
    {
    case QtRPCItem::ConnectionPeer:
    {
        tcpServer_->setMaxPendingConnections(1);
        break;
    }
    case QtRPCItem::ConnectionRoot:
    {
        break;
    }
    case QtRPCItem::ConnectionBransh:
    {
        break;
    }
    case QtRPCItem::ConnectionDistribute:
    {
        tcpServer_->setMaxPendingConnections(3);
        break;
    }
    default:
    {
        qFatal("unknow connection type.");
        abort();
        break;
    }
    }

}

void QtRPCNetworkTcpPrivate::clearConnections(QTcpSocket *socket)
{
    if (socket == 0)
    {
        for (QMap<QTcpSocket*, MsgSection*>::iterator itr = sectionCtrls_.begin(); itr != sectionCtrls_.end(); ++itr)
        {
            if (itr.key() == tcpSocketOut_.data())
            {
                tcpSocketOut_->blockSignals(true);
                tcpSocketOut_->close();
                tcpSocketOut_.clear();
                delete itr.value();
                continue;
            }

            itr.key()->blockSignals(true);
            itr.key()->close();
            delete itr.key();

            delete itr.value();
        }
        sectionCtrls_.clear();
        return;
    }
    QMap<QTcpSocket*, MsgSection*>::iterator itr = sectionCtrls_.find(socket);
    if (itr == sectionCtrls_.end())
        return;

    delete itr.value();
    sectionCtrls_.erase(itr);


    if (socket != 0 && tcpSocketOut_.data() != socket)
    {
        socket->blockSignals(true);
        socket->close();
        delete socket;
        socket = 0;
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


void QtRPCNetworkTcpPrivate::recreateConnection(QTcpSocket *socket)
{
    if (exitMark)
        return;

    this->clearConnections(socket);

    QEventLoop loop;
    QTimer::singleShot(1000, &loop, SLOT(quit()));
    loop.exec();

    if (exitMark)
        return;

    createServer();
    searcher_.startSearch("0.0.0.0", myPort_);
}

void QtRPCNetworkTcpPrivate::onGotPeer(const QString &/*address*/, const QString &ip, quint16 port)
{
    connectToServer(ip, port);
}

void QtRPCNetworkTcpPrivate::onGotMsg(int msgid, const QByteArray &data, int /*socketId*/)
{
    MsgSection *sectionCtrl = (MsgSection *)this->sender();
    emit q_ptr->doGotMsg(msgid, data, sectionCtrl->descriptor());
}

void QtRPCNetworkTcpPrivate::connectToServer(const QString &address, quint16 port)
{
    //if connect failed, "disconnected" will trigger.
    createClient();
    tcpSocketOut_->setSocketOption(QAbstractSocket::LowDelayOption, true);
    tcpSocketOut_->connectToHost(address, port);
}

void QtRPCNetworkTcpPrivate::connectSignals(QObject *obj)
{
    QTcpSocket *socket = (QTcpSocket*)obj;
    if (sectionCtrls_.contains(socket))
    {
        return;
    }

    MsgSection *sectionCtrl_ = new MsgSection((int)socket, socket);
    sectionCtrl_->setParent(this);
    sectionCtrls_.insert(socket, sectionCtrl_);

    connect(sectionCtrl_, SIGNAL(doGotMsg(int,QByteArray,int)),
            this, SLOT(onGotMsg(int,QByteArray,int)));

    connect(obj, SIGNAL(connected()),
            this, SLOT(connected()));
    connect(obj, SIGNAL(readyRead()),
            this, SLOT(readReady()));
    connect(obj, SIGNAL(disconnected()),
            this, SLOT(disconnected()));
    connect(obj, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));
}

void QtRPCNetworkTcpPrivate::writeSections(int id, const QByteArray &data, int mySocketId)
{
    for (QMap<QTcpSocket*, MsgSection*>::iterator itr = sectionCtrls_.begin(); itr != sectionCtrls_.end(); ++itr)
    {
        if (mySocketId == 0)
        {
            itr.value()->write(id, data, 0);
            continue;
        }

        if ((int)itr.key() == mySocketId)
        {
            itr.value()->write(id, data, 0);
            break;
        }
    }
}


