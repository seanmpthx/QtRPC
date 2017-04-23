#include "QtRPCNetworkLocalPrivate.h"
#include "../../include/QtRPCManager.h"
#include "QtRPCNetworkLocal.h"
#include <QNetworkInterface>
#include <QMetaMethod>
#include <QEventLoop>

QtRPCNetworkLocalPrivate::QtRPCNetworkLocalPrivate(QtRPCNetworkLocal *q, const QString &identifier, int ctype) : q_ptr(q),
    identifier_(identifier), peerAddress_(QUuid::createUuid().toString()), searcher_(identifier, ctype),
    connectionType(ctype), exitMark(false)
{
    connect(&searcher_, SIGNAL(doGotPeer(QString,QString,quint16)),
            this, SLOT(onGotPeer(QString,QString,quint16)));

    createServer();

    searcher_.startSearch(peerAddress_, 0);

    qRegisterMetaType<QLocalSocket*>("QLocalSocket*");
}

QtRPCNetworkLocalPrivate::~QtRPCNetworkLocalPrivate()
{
    qDebug() << "~QtRPCNetworkLocalPrivate";
    exitMark = true;
    clearConnections();
}


void QtRPCNetworkLocalPrivate::writeMsg(int msgid, const QByteArray &data, int socketId)
{
    writeSections(msgid, data, socketId);
}

void QtRPCNetworkLocalPrivate::newConnection()
{
    ++currentPaddings;
    qDebug() << "new connection.1";
    QLocalSocket *localSocketIn = localServer_->nextPendingConnection();
    connectSignals(localSocketIn);

    searcher_.stopSearch();

    switch(connectionType)
    {
    case QtRPCItem::ConnectionPeer:
    {
        localServer_->close();
        break;
    }
    case QtRPCItem::ConnectionRoot:
    {
        break;
    }
    case QtRPCItem::ConnectionBransh:
    {
        localServer_->close();
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

void QtRPCNetworkLocalPrivate::readReady()
{
    QLocalSocket *socket = (QLocalSocket*)this->sender();
    QMap<QLocalSocket*, MsgSection*>::iterator itr = sectionCtrls_.find(socket);
    if (itr == sectionCtrls_.end())
        return;
    //qDebug() << "read ready";
    itr.value()->read();
}

void QtRPCNetworkLocalPrivate::connected()
{
    if (localServer_)
        localServer_->close();

    //qDebug() << "connected";
    searcher_.stopSearch();
}

void QtRPCNetworkLocalPrivate::disconnected()
{
    if (currentPaddings > 0)
        --currentPaddings;

    QLocalSocket *socket = (QLocalSocket*)this->sender();
    //qDebug() << "disconnected" << this->sender();

    QMetaObject::invokeMethod(this, "recreateConnection", Qt::QueuedConnection, Q_ARG(QLocalSocket *, socket));
}

void QtRPCNetworkLocalPrivate::error(QLocalSocket::LocalSocketError socketError)
{
    qDebug() << "local socket error:" << socketError;
}

void QtRPCNetworkLocalPrivate::createClient()
{
    localSocketOut_ = QSharedPointer<QLocalSocket>(new QLocalSocket());
    QObject *obj = localSocketOut_.data();
    connectSignals(obj);
}

void QtRPCNetworkLocalPrivate::createServer()
{
    if (connectionType == QtRPCItem::ConnectionBransh)
    {
        return;
    }

    localServer_ = QSharedPointer<QLocalServer>(new QLocalServer());

    if (!localServer_->listen(peerAddress_))
    {
        //qDebug() << "local server listen failed:" << peerAddress_;
        return;
    }

    //qDebug() << "create local server at:" << peerAddress_ << " " << this;

    connect(localServer_.data(), SIGNAL(newConnection()),
            this, SLOT(newConnection()));

    switch(connectionType)
    {
    case QtRPCItem::ConnectionPeer:
    {
        localServer_->setMaxPendingConnections(1);
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
        localServer_->setMaxPendingConnections(3);
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

void QtRPCNetworkLocalPrivate::clearConnections(QLocalSocket *socket)
{
    if (socket == 0)
    {
        for (QMap<QLocalSocket*, MsgSection*>::iterator itr = sectionCtrls_.begin(); itr != sectionCtrls_.end(); ++itr)
        {
            if (itr.key() == localSocketOut_.data())
            {
                localSocketOut_->blockSignals(true);
                localSocketOut_->close();
                localSocketOut_.clear();
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
    QMap<QLocalSocket*, MsgSection*>::iterator itr = sectionCtrls_.find(socket);
    if (itr == sectionCtrls_.end())
        return;

    delete itr.value();
    sectionCtrls_.erase(itr);

    if (socket != 0 && localSocketOut_.data() != socket)
    {
        socket->blockSignals(true);
        socket->close();
        delete socket;
        socket = 0;
    }

    if (localSocketOut_)
    {
        localSocketOut_->blockSignals(true);
        localSocketOut_->close();
        localSocketOut_.clear();
    }

    if (localServer_)
    {
        QLocalServer::removeServer(peerAddress_);
        localServer_->blockSignals(true);
        localServer_->close();
        localServer_.clear();
    }

    searcher_.stopSearch();
}


void QtRPCNetworkLocalPrivate::recreateConnection(QLocalSocket *socket)
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
    searcher_.startSearch(peerAddress_, 0);
}

void QtRPCNetworkLocalPrivate::onGotPeer(const QString &address, const QString &/*ip*/, quint16 /*port*/)
{
    connectToServer(address);
}

void QtRPCNetworkLocalPrivate::onGotMsg(int msgid, const QByteArray &data, int /*socketId*/)
{
    MsgSection *sectionCtrl = (MsgSection *)this->sender();

    emit q_ptr->doGotMsg(msgid, data, sectionCtrl->descriptor());
}


void QtRPCNetworkLocalPrivate::connectToServer(const QString &address)
{
    createClient();
    localSocketOut_->connectToServer(address);
}

void QtRPCNetworkLocalPrivate::connectSignals(QObject *obj)
{
    QLocalSocket *socket = (QLocalSocket*)obj;
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
    connect(obj, SIGNAL(error(QLocalSocket::LocalSocketError)),
            this, SLOT(error(QLocalSocket::LocalSocketError)));
}

void QtRPCNetworkLocalPrivate::writeSections(int id, const QByteArray &data, int socketId)
{
    for (QMap<QLocalSocket*, MsgSection*>::iterator itr = sectionCtrls_.begin(); itr != sectionCtrls_.end(); ++itr)
    {
        if (socketId == 0)
        {
            itr.value()->write(id, data, 0);
            continue;
        }

        if ((int)itr.key() == socketId)
        {
            itr.value()->write(id, data, 0);
            break;
        }
    }
}


