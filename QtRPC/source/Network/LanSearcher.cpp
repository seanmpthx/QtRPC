#include "LanSearcher.h"
#include <QUuid>
#include <QCoreApplication>
#include <QStringList>
#include <QDataStream>
#include <QNetworkInterface>

LanSearcher::LanSearcher() : localSearchPort_(0), uuid_(QUuid::createUuid())
{
    connect(&timerSearch_, SIGNAL(timeout()),
            this, SLOT(timerSearchMsg()));
    timerSearch_.setInterval(1000);
}

void LanSearcher::startSearch(const QString &myIp, quint16 port)
{
    //1st step, request exist server
    udpSocketSearch_ = QSharedPointer<QUdpSocket>(new QUdpSocket());

    connect(udpSocketSearch_.data(), SIGNAL(readyRead()),
            this, SLOT(searchReadReady()));
    //bind receive port
    for (localSearchPort_ = 8013; localSearchPort_ < 8020; ++localSearchPort_)
    {
        if (!udpSocketSearch_->bind(localSearchPort_, QUdpSocket::DontShareAddress))
            continue;
        break;
    }

    QList<QHostAddress> allAddress = QNetworkInterface::allAddresses();
    //multicast to search. if tcp, 0.0.0.0 is all address.
    foreach(const QHostAddress &address, allAddress)
    {
        udpSocketSearch_->joinMulticastGroup(address);
    }

    qDebug() << "1st step, request exist server at port:" << localSearchPort_;
    myIp_ = myIp;
    myPort_ = port;

    timerSearchMsg();
    timerSearch_.start();
}

void LanSearcher::stopSearch()
{
    stopSearchSlot();
}

void LanSearcher::stopSearchSlot()
{
    if (udpSocketSearch_)
    {
        udpSocketSearch_->blockSignals(true);
        udpSocketSearch_.clear();
    }

    qDebug() << "stop to search";
    timerSearch_.stop();
}

void LanSearcher::searchReadReady()
{
    int size = udpSocketSearch_->bytesAvailable();
    QByteArray data(size, 0);

    QHostAddress address;
    quint16 portIn;
    udpSocketSearch_->readDatagram(data.data(), size, &address, &portIn);
    QDataStream streamRead(data);
    streamRead.setVersion(QDataStream::Qt_4_8);

    QUuid uuid;
    int id = 0;
    quint16 port;
    QString identifier;
    QString peerAddress; //local only
    streamRead >> id >> identifier >> peerAddress >> port >> uuid;

    if (uuid == uuid_)
    {
        return;
    }
    if (singleApplicationId() == identifier)
    {
        return;
    }


    qDebug() << "search ready";

    if (id == SearchRequest)
    {
        qDebug() << "request:" << " local:" << peerAddress << " my:" << myIp_ << " find:" << address.toString();
        data.clear();
        QDataStream streamWrite(&data, QIODevice::WriteOnly);
        streamWrite.setVersion(QDataStream::Qt_4_8);
        streamWrite << (int)SearchResponse << singleApplicationId() << myIp_ << myPort_ << uuid_;
        udpSocketSearch_->writeDatagram(data, address, portIn);
    }
    else if (id == SearchResponse)
    {
        //2nd step, if response, connect to server.
        qDebug() << "found peer, connecting..." << " local:" << peerAddress << " get:" << address.toString() << " port:" << port;

        emit doGotPeer(peerAddress, address.toString(), port);
        QTimer::singleShot(0, this, SLOT(stopSearchSlot()));
    }

}

void LanSearcher::timerSearchMsg()
{
    if (!udpSocketSearch_)
    {
        qDebug() << "destory udp";
        timerSearch_.stop();
        return;
    }

    QByteArray data;
    {
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream << (int)SearchRequest << singleApplicationId() << QString("") << myPort_ << uuid_;
    }

    for (int i = 8013; i < 8020; ++i)
    {
        udpSocketSearch_->writeDatagram(data, QHostAddress::Broadcast, i);
    }
    qDebug() << "broadcast msg to search." << timerSearch_.isActive() << timerSearch_.interval();
}

QString LanSearcher::singleApplicationId()
{
    QString identifier = qApp->arguments().first() + QString::number((qint64)qApp);
    return identifier;
}

