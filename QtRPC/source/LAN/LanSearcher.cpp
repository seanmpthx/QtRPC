#include "LanSearcher.h"
#include "../../include/QtRPCManager.h"
#include <QUuid>
#include <QCoreApplication>
#include <QStringList>
#include <QDataStream>
#include <QNetworkInterface>

QDataStream &operator << (QDataStream &stream, const SearchData &data)
{
    stream << data.msgId << data.applicationId << data.myIp << data.myPort << data.uuid << data.connectionType;
    return stream;
}

QDataStream &operator >> (QDataStream &stream, SearchData &data)
{
    stream >> data.msgId >> data.applicationId >> data.myIp >> data.myPort >> data.uuid >> data.connectionType;
    return stream;
}


LanSearcher::LanSearcher(const QString &identifier, int ctype) : identifier_(identifier),
    connectionType_(ctype), localSearchPort_(0), uuid_(QUuid::createUuid())
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
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        if (!udpSocketSearch_->bind(QHostAddress::AnyIPv4, localSearchPort_, QUdpSocket::DontShareAddress))
            continue;
#else
        if (!udpSocketSearch_->bind(QHostAddress::Any, localSearchPort_, QUdpSocket::DontShareAddress))
            continue;
#endif

        break;
    }

    QList<QHostAddress> allAddress = QNetworkInterface::allAddresses();
    //multicast to search. if tcp, 0.0.0.0 is all address.
    foreach(const QHostAddress &address, allAddress)
    {
        udpSocketSearch_->joinMulticastGroup(address);
    }


    myIp_ = myIp;
    myPort_ = port;

    switch(connectionType_)
    {
    case QtRPCItem::ConnectionRoot:
        break;
    case QtRPCItem::ConnectionPeer:
    case QtRPCItem::ConnectionBransh:
    case QtRPCItem::ConnectionDistribute:
    {
        //qDebug() << "1st step, request exist server at port:" << localSearchPort_;
        timerSearchMsg();
        timerSearch_.start();
        break;
    }
    }
}

void LanSearcher::stopSearch()
{
    stopSearchSlot();
}

void LanSearcher::closeSearch()
{
    if (udpSocketSearch_)
    {
        udpSocketSearch_->blockSignals(true);
        udpSocketSearch_.clear();
    }
    timerSearch_.stop();
}

void LanSearcher::stopSearchSlot()
{
    switch(connectionType_)
    {
    case QtRPCItem::ConnectionRoot:
        break;
    case QtRPCItem::ConnectionPeer:
    case QtRPCItem::ConnectionBransh:
    {
        if (udpSocketSearch_)
        {
            udpSocketSearch_->blockSignals(true);
            udpSocketSearch_.clear();
        }
        timerSearch_.stop();
        break;
    }
    case QtRPCItem::ConnectionDistribute:
    {
        timerSearch_.stop();
        break;
    }
    }

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

    SearchData sdataResopnse;
    streamRead >> sdataResopnse;

    if (sdataResopnse.identifier != identifier_) //peer check
        return;
    if (sdataResopnse.uuid == uuid_) //self check
        return;
    if (singleApplicationId() == sdataResopnse.applicationId) //application check
        return;

    switch (sdataResopnse.connectionType)
    {
    case QtRPCItem::ConnectionPeer:
        if (connectionType_ != QtRPCItem::ConnectionPeer)
            return;
        break;
    case QtRPCItem::ConnectionRoot:
        if (connectionType_ != QtRPCItem::ConnectionBransh)
            return;
        break;
    case QtRPCItem::ConnectionBransh:
        if (connectionType_ != QtRPCItem::ConnectionRoot)
            return;
        break;
    case QtRPCItem::ConnectionDistribute:
        if (connectionType_ != QtRPCItem::ConnectionDistribute)
            return;
        break;
    }


    if (sdataResopnse.msgId == SearchRequest)
    {
        //qDebug() << "request:" << " local:" << sdataResopnse.myIp << " my:" << myIp_ << " find:" << address.toString();
        data.clear();
        QDataStream streamWrite(&data, QIODevice::WriteOnly);
        streamWrite.setVersion(QDataStream::Qt_4_8);

        SearchData sdata;
        sdata.applicationId = singleApplicationId();
        sdata.connectionType = connectionType_;
        sdata.msgId = SearchResponse;
        sdata.myIp = myIp_;
        sdata.myPort = myPort_;
        sdata.uuid = uuid_;
        sdata.identifier = identifier_;

        streamWrite << sdata;
        udpSocketSearch_->writeDatagram(data, address, portIn);
    }
    else if (sdataResopnse.msgId == SearchResponse)
    {
        //2nd step, if response, connect to server.
        //qDebug() << "found peer, connecting..." << " local:" << sdataResopnse.myIp << " get:" << address.toString() << " port:" << sdataResopnse.myPort;

        udpSocketSearch_->blockSignals(true);
        udpSocketSearch_->close();
        emit doGotPeer(sdataResopnse.myIp, address.toString(), sdataResopnse.myPort);
    }

}

void LanSearcher::timerSearchMsg()
{
    if (!udpSocketSearch_)
    {
        timerSearch_.stop();
        return;
    }

    QByteArray data;
    {
        QDataStream stream(&data, QIODevice::WriteOnly);
        stream.setVersion(QDataStream::Qt_4_8);

        SearchData sdata;
        sdata.applicationId =  singleApplicationId();
        sdata.connectionType = connectionType_;
        sdata.msgId = SearchRequest;
        sdata.myIp = myIp_;
        sdata.myPort = myPort_;
        sdata.uuid = uuid_;
        sdata.identifier = identifier_;

        stream << sdata;
    }

    for (int i = 8013; i < 8020; ++i)
    {
        udpSocketSearch_->writeDatagram(data, QHostAddress::Broadcast, i);
    }
}

QString LanSearcher::singleApplicationId()
{
    if (applicateionId.isEmpty())
        applicateionId = qApp->arguments().first() + QString::number((qint64)qApp);
    return applicateionId;
}

