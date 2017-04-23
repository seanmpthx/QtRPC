#ifndef QTRPCMANAGERPRIVATE_H
#define QTRPCMANAGERPRIVATE_H

#include <QObject>
#include <QHash>
#include <QUuid>

class QtRPCItem;
class QtRPCManager;
class QtRPCManagerPrivate
{
public:
    QtRPCManagerPrivate(QtRPCManager *mgr);

private:
    QtRPCItem *item(const QString &id);
    void addItem(const QString &id, QtRPCItem *item);
    void releaseBeforeEventLoopExit();

private:
    friend class QtRPCItem;
    Q_DECLARE_PUBLIC(QtRPCManager)
    QtRPCManager * const q_ptr;

    QHash<QString, QtRPCItem *> rpcItems;
};

#endif // QTRPCMANAGERPRIVATE_H
