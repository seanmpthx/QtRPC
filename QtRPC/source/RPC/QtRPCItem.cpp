#include "../../include/QtRPCManager.h"
#include "QtRPCManagerPrivate.h"
#include "QtRPCItemPrivate.h"
#include <QMetaObject>
#include <QDebug>

QtRPCItem::QtRPCItem(SocketType type, ConnectionType ctype, const QString &peerId) :
    d_ptr(new QtRPCItemPrivate(this, type, ctype, peerId))
{

}

QtRPCItem::QtRPCItem(QtRPCNetwork *network) : d_ptr(new QtRPCItemPrivate(this, network))
{

}

bool QtRPCItem::connect(int functionId, QObject *obj, const char *slot, int timeout)
{
    Q_D(QtRPCItem);
    d->registerSlot(obj, slot, functionId, timeout);
    return true;
}

void QtRPCItem::registerSlot(QObject *obj, const char *slot, int uniqueId, int timeout)
{
    Q_D(QtRPCItem);
    d->registerSlot(obj, slot, uniqueId, timeout);
}


QtRPCItem::~QtRPCItem()
{
    delete d_ptr;
}

QtRPCItem::SocketType QtRPCItem::socketType() const
{
    const Q_D(QtRPCItem);
    return (SocketType)d->socketType();
}

void QtRPCItem::setMsgData(int unique, const QByteArray &data, int descriptor)
{
    Q_UNUSED(unique);
    Q_UNUSED(data);
    Q_UNUSED(descriptor);
}

void QtRPCItem::writeMsg(int msgid, const QByteArray &data, int descriptor)
{
    Q_D(QtRPCItem);
    //d->writeMsg(msgid, data, descriptor);
    QMetaObject::invokeMethod(d, "writeMsg", Qt::QueuedConnection,
                              Q_ARG(int, msgid), Q_ARG(QByteArray, data), Q_ARG(int, descriptor));
}

QObject *QtRPCItem::object(int funcId)
{
    Q_D(QtRPCItem);
    return d->object(funcId);
}

const char *QtRPCItem::slotName(int funcId)
{
    Q_D(QtRPCItem);
    return d->slotName(funcId);
}
