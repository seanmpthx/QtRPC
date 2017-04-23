#include "../../include/QtRPCManager.h"
#include "QtRPCManagerPrivate.h"
#include "QtRPCItemPrivate.h"

QtRPCManager::QtRPCManager() : d_ptr(new QtRPCManagerPrivate(this))
{

}

QtRPCItem *QtRPCManager::item(const QString &id)
{
    Q_D(QtRPCManager);
    return d->item(id);
}

void QtRPCManager::addItem(const QString &id, QtRPCItem *item)
{
    Q_D(QtRPCManager);
    d->addItem(id, item);
}

QtRPCManager::~QtRPCManager()
{

}

QtRPCManager &QtRPCManager::instance()
{
    static QtRPCManager s_QtRPCManager;
    return s_QtRPCManager;
}

void QtRPCManager::releaseBeforeEventLoopExit()
{
    Q_D(QtRPCManager);
    d->releaseBeforeEventLoopExit();
}

