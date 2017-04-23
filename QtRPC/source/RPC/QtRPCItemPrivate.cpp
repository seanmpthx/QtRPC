#include "QtRPCItemPrivate.h"
#include "../../include/QtRPCManager.h"
#include "../LAN/QtRPCNetworkLocal.h"
#include "../LAN/QtRPCNetworkTcp.h"
#include <QDebug>

QtRPCItemPrivate::QtRPCItemPrivate(QtRPCItem *q, int stype, int ctype, const QString &peerId) : q_ptr(q),
    socketType_(stype), connectiontype_(ctype), peerId_(peerId)
{
    create();
}

QtRPCItemPrivate::QtRPCItemPrivate(QtRPCItem *q, QtRPCNetwork *network) : q_ptr(q),
    socketType_(QtRPCItem::SocketCustom), connectiontype_(QtRPCItem::ConnectionUnknow)
{
    if (network == 0)
    {
        qFatal("network is null.");
        return;
    }
    networkItem = QSharedPointer<QtRPCNetwork>(network);

    connect(networkItem.data(), SIGNAL(doGotMsg(int,QByteArray,int)),
            this, SLOT(onGotMsg(int,QByteArray,int)));
}

QtRPCItemPrivate::~QtRPCItemPrivate()
{
    emit doExit();
    networkItem.clear();
}

int QtRPCItemPrivate::socketType() const
{
    return socketType_;
}

void QtRPCItemPrivate::setMsgData(int unique, const QByteArray &data, int socketId)
{
    Q_Q(QtRPCItem);
    emit doGotMsg(unique, data, socketId);
    q->setMsgData(unique, data, socketId);
}

void QtRPCItemPrivate::registerSlot(QObject *obj, const QByteArray &slot, int uniqueId, int timeout)
{
    Q_UNUSED(timeout);
    slotsCache.insert(uniqueId, QPair<QObject*, QByteArray>(obj, slot));
}

void QtRPCItemPrivate::addFunction(void *classPtr, void *functionPtr, int uniqueId, int timeout)
{
    Q_UNUSED(timeout);
    functions_.insert(uniqueId, QPair<void*, void*>(classPtr, functionPtr));
}

void QtRPCItemPrivate::writeMsg(int unique, const QByteArray &data, int socketId)
{
    networkItem->writeMsg(unique, data, socketId);
}

void QtRPCItemPrivate::callFunction(int funcId, const QList<void *> &args, void *ret)
{
    Q_UNUSED(funcId);
    Q_UNUSED(args);
    Q_UNUSED(ret);
    //    qDebug() << "call function";
    //    QMap<int, QPair<void*, void*> >::iterator itr = functions_.find(funcId);
    //    if (itr == functions_.end())
    //    {
    //        qWarning() << "can't find function, id : " << funcId;
    //        return;
    //    }
    //    QPair<void*, void*> &pair = itr.value();

    //    void *cls = pair.first;
    //    void *func = (void*)(*((qint32*)pair.second));

    //    int count = args.count();

    //    for (int i = 0; i < count; ++i)
    //    {
    //        void *temp = args.at(i);
    //        __asm mov eax, temp;
    //        __asm push eax;
    //    }
    //    __asm
    //    {
    //        mov ecx,cls;
    //        call func;
    //        mov ret,eax;
    //    }
}

void QtRPCItemPrivate::create()
{
    switch(socketType_)
    {
    case QtRPCItem::SocketLocal:
        networkItem = QSharedPointer<QtRPCNetwork>(new QtRPCNetworkLocal(peerId_, connectiontype_));
        break;
    case QtRPCItem::SocketTcp:
        networkItem = QSharedPointer<QtRPCNetwork>(new QtRPCNetworkTcp(peerId_, connectiontype_));
        break;
    case QtRPCItem::SocketUdp:
        break;
    }

    connect(networkItem.data(), SIGNAL(doGotMsg(int,QByteArray,int)),
            this, SLOT(onGotMsg(int,QByteArray,int)));
}

QObject *QtRPCItemPrivate::object(int funcId)
{
    QMap<int, QPair<QObject*, QByteArray> >::iterator itr = slotsCache.find(funcId);
    if (itr == slotsCache.end())
        return 0;
    return itr.value().first;
}

const char *QtRPCItemPrivate::slotName(int funcId)
{
    QMap<int, QPair<QObject*, QByteArray> >::iterator itr = slotsCache.find(funcId);
    if (itr == slotsCache.end())
        return 0;
    return itr.value().second.data();
}

void QtRPCItemPrivate::onGotMsg(int unique, const QByteArray &data, int descriptor)
{
    this->setMsgData(unique, data, descriptor);
}
