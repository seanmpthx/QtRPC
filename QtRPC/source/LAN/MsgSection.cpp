#include "MsgSection.h"
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include <QTcpSocket>
#include <QLocalSocket>
#include <QUdpSocket>
#include <QDataStream>

MsgSection::MsgSection(int descriptor, QIODevice *device) : descriptor_(descriptor), device(device), packageSize(0)
{

}

int MsgSection::descriptor() const
{
    return descriptor_;
}

void MsgSection::read()
{
    quint32 size = 0;
    do
    {
        size = device->bytesAvailable();
        if (size < sizeof(quint32))
        {
            packageSize = 0;
            return;
        }
        QDataStream stream(device);
        if (packageSize == 0)
        {
            stream.setVersion(QDataStream::Qt_4_8);
            stream >> packageSize;
        }
        size = device->bytesAvailable();
        if (size < packageSize - sizeof(quint32))
        {
            return;
        }

        QByteArray data;
        stream >> data;

        QDataStream streamData(data);

        qint32 socketId = 0;
        qint32 msgId = 0;
        qint32 subId = 0;
        qint32 msgTotalSize = 0;
        QByteArray msg;
        streamData >> socketId >> msgId >> subId >> msgTotalSize >> msg;

        if (msgTotalSize == msg.size())
        {
            packageSize = 0;
            emit doGotMsg(msgId, msg, socketId);
            return;
        }


        QHash<int, QByteArray>::iterator itr = packages.find(subId);
        if (itr == packages.end())
        {
            itr = packages.insert(subId, QByteArray());
        }
        QByteArray &info = itr.value();
        info.append(msg);
        packageSize = 0;

        if (info.size() >= msgTotalSize && msgTotalSize > 0)
        {
            emit doGotMsg(msgId, info, socketId);
            packages.erase(itr);
            return;
        }

    } while(size > sizeof(quint32));
}

void MsgSection::write(int msgId, const QByteArray &data, int socketId)
{
    int totalSize = data.size();
    QList<QByteArray> dataSections;
    int index = 0;
    do
    {
        QByteArray temp = data.mid(index, 60000);
        dataSections.append(temp);
        index += temp.size();
    } while(index < data.size() - 1);

    static quint32 packageId = 1;

    for (QList<QByteArray>::iterator itr = dataSections.begin(); itr != dataSections.end(); ++itr)
    {
        QByteArray &section = *itr;

        QByteArray data;

        {
            QDataStream stream(&data, QIODevice::WriteOnly);
            stream << (quint32)socketId << (quint32)msgId << (quint32)packageId << (quint32)totalSize << section;
        }

        section = data;
    }

    ++packageId;
    if (packageId > 0xFFFF)
        packageId = 1;

    for (QList<QByteArray>::const_iterator itr = dataSections.begin(); itr != dataSections.end(); ++itr)
    {
        const QByteArray &section = *itr;

        QByteArray dataSend;
        QDataStream streamSend(&dataSend, QIODevice::WriteOnly);
        streamSend << (quint32)0 << section;
        streamSend.device()->seek(0);
        streamSend << dataSend.size();

        device->write(dataSend);

        //debug...
        //QEventLoop loop;
        //QTimer::singleShot(100, &loop, SLOT(quit()));
        //loop.exec();
    }
}
