#include "MsgSection.h"
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include <QTcpSocket>
#include <QLocalSocket>
#include <QUdpSocket>
#include <QDataStream>

MsgSection::MsgSection(QIODevice *device) : device(device), packageSize(0)
{

}

void MsgSection::read()
{
    quint32 size = 0;
    do
    {
        size = device->bytesAvailable();
        if (size < sizeof(quint32))
        {
            qDebug() << "return bytes:" << size;
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
            qDebug() << "return packageSize:" << (packageSize - sizeof(quint32))  << " ? " << size;
            return;
        }

        QByteArray data;
        stream >> data;
        qDebug() << "data size:" << size << " " << data.size();

        QDataStream streamData(data);

        qint32 msgId = 0;
        qint32 subId = 0;
        qint32 msgTotalSize = 0;
        QByteArray msg;
        streamData >> msgId >> subId >> msgTotalSize >> msg;

        qDebug() << "total size:" << msgTotalSize;
        if (msgTotalSize == msg.size())
        {
            packageSize = 0;
            qDebug() << "fin1" << msgTotalSize << " " << msg.toHex();
            emit doGotMsg(msgId, msg);
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

        //static int indexCount = 0;
        //qDebug() << "msg size;" << info.size()  << "/" << msgTotalSize << " count:" << indexCount++;

        if (info.size() >= msgTotalSize && msgTotalSize > 0)
        {
            qDebug() << "msg fin2";
            emit doGotMsg(msgId, info);
            packages.erase(itr);
            return;
        }

    } while(size > sizeof(quint32));
}

void MsgSection::write(int msgId, const QByteArray &data)
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
            stream << (quint32)msgId << (quint32)packageId << (quint32)totalSize << section;
        }

        section = data;
    }

    ++packageId;
    if (packageId > 0xFFFF)
        packageId = 1;

    //qDebug() << "msg send;" << totalSize << " pack count:" << dataSections.count();
    for (QList<QByteArray>::const_iterator itr = dataSections.begin(); itr != dataSections.end(); ++itr)
    {
        const QByteArray &section = *itr;

        QByteArray dataSend;
        QDataStream streamSend(&dataSend, QIODevice::WriteOnly);
        streamSend << (quint32)0 << section;
        streamSend.device()->seek(0);
        streamSend << dataSend.size();

        //qDebug() << "write:" << section.size();
        device->write(dataSend);
        qDebug() << "write:" << dataSend.toHex();

        QEventLoop loop;
        QTimer::singleShot(1, &loop, SLOT(quit()));
        loop.exec();
    }
}
