#include "QtRPCManagerPrivate.h"
#include "../../include/QtRPCManager.h"
#include "QtRPCItemPrivate.h"

QtRPCManagerPrivate::QtRPCManagerPrivate(QtRPCManager *mgr) : q_ptr(mgr)
{

}

QtRPCItem *QtRPCManagerPrivate::item(const QString &id)
{
    QHash<QString, QtRPCItem *>::iterator itr = rpcItems.find(id);
    if (itr == rpcItems.end())
        return 0;
    return itr.value();
}

void QtRPCManagerPrivate::addItem(const QString &id, QtRPCItem *item)
{
    rpcItems.insert(id, item);
}

void QtRPCManagerPrivate::releaseBeforeEventLoopExit()
{
    for (QHash<QString, QtRPCItem *>::iterator itr = rpcItems.begin();
         itr != rpcItems.end(); ++itr)
    {
        QtRPCItem *item = itr.value();
        delete item;
    }
}
