#include "QtRPCNetworkLocalPrivate.h"
#include "../../include/QtRPCManager.h"
#include "QtRPCNetworkLocal.h"
#include <QNetworkInterface>
#include <QMetaMethod>

QtRPCNetworkLocalPrivate::QtRPCNetworkLocalPrivate(QtRPCNetworkLocal *q, int maxPeers) : q_ptr(q),
    peerAddress_(QUuid::createUuid().toString()), localSocketIn_(NULL), sectionCtrl_(NULL)
{
    connect(&searcher_, SIGNAL(doGotPeer(QString,QString,quint16)),
            this, SLOT(onGotPeer(QString,QString,quint16)));

    createServer();
    searcher_.startSearch(peerAddress_, 0);

}

QtRPCNetworkLocalPrivate::~QtRPCNetworkLocalPrivate()
{
    clearConnections();
}


void QtRPCNetworkLocalPrivate::writeMsg(int msgid, const QByteArray &data)
{
    if (localSocketIn_ != NULL)
    {
        qDebug() << "write msg 1";
        writeSections(msgid, data);
    }

    if (localSocketOut_)
    {
        qDebug() << "write msg 2";
        writeSections(msgid, data);
    }

    qDebug() << "not connected";

}

void QtRPCNetworkLocalPrivate::newConnection()
{
    qDebug() << "new connection.1";
    localSocketIn_ = localServer_->nextPendingConnection();
    connectSignals(localSocketIn_);
    localServer_->close();
    searcher_.stopSearch();
}

void QtRPCNetworkLocalPrivate::readReady()
{
    if (sectionCtrl_ == NULL)
        return;
    qDebug() << "read ready";
    sectionCtrl_->read();
}

void QtRPCNetworkLocalPrivate::connected()
{
    qDebug() << "connected";
    searcher_.stopSearch();
}

void QtRPCNetworkLocalPrivate::disconnected()
{
    qDebug() << "disconnected" << this->sender();
    QTimer::singleShot(0, this, SLOT(recreateConnection()));
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
    localServer_ = QSharedPointer<QLocalServer>(new QLocalServer());
    localServer_->setMaxPendingConnections(1);

    if (!localServer_->listen(peerAddress_))
    {
        qDebug() << "local server listen failed:" << peerAddress_;
        return;
    }

    qDebug() << "create local server at:" << peerAddress_ << " " << this;

    connect(localServer_.data(), SIGNAL(newConnection()),
            this, SLOT(newConnection()));
}

void QtRPCNetworkLocalPrivate::clearConnections()
{
    QLocalServer::removeServer(peerAddress_);

    if (sectionCtrl_ != 0)
    {
        delete sectionCtrl_;
        sectionCtrl_ = 0;
    }

    if (localSocketIn_ != 0)
    {
        localSocketIn_->blockSignals(true);
        localSocketIn_->close();
        delete localSocketIn_;
        localSocketIn_ = 0;
    }

    if (localSocketOut_)
    {
        localSocketOut_->blockSignals(true);
        localSocketOut_->close();
        localSocketOut_.clear();
    }

    if (localServer_)
    {
        localServer_->blockSignals(true);
        localServer_->close();
        localServer_.clear();
    }

    searcher_.stopSearch();
}


void QtRPCNetworkLocalPrivate::recreateConnection()
{
    this->clearConnections();


    createServer();
    searcher_.startSearch(peerAddress_, 0);
}

void QtRPCNetworkLocalPrivate::onGotPeer(const QString &address, const QString &/*ip*/, quint16 /*port*/)
{
    qDebug() << "got peer," << "ip:" << address;
    connectToServer(address);
}

void QtRPCNetworkLocalPrivate::connectToServer(const QString &address)
{
    if (localServer_)
        localServer_->close();

    createClient();
    localSocketOut_->connectToServer(address);
}

void QtRPCNetworkLocalPrivate::connectSignals(QObject *obj)
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
    connect(obj, SIGNAL(error(QLocalSocket::LocalSocketError)),
            this, SLOT(error(QLocalSocket::LocalSocketError)));
}

void QtRPCNetworkLocalPrivate::writeSections(int id, const QByteArray &data)
{
    if (sectionCtrl_ == 0)
        return;

    sectionCtrl_->write(id, data);
}


