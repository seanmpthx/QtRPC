#ifndef QTRPCNETWORKLOCAL_H
#define QTRPCNETWORKLOCAL_H

#include <QUuid>
#include <QObject>
#include "../../include/QtRPCNetwork.h"

class QIODevice;
class QtRPCNetworkLocalPrivate;

class QtRPCNetworkLocal : public QtRPCNetwork
{
    Q_OBJECT
public:
    QtRPCNetworkLocal(int maxPeers = 1);
    ~QtRPCNetworkLocal();

    virtual void writeMsg(int msgId, const QByteArray &data);

private:
    Q_DECLARE_PRIVATE(QtRPCNetworkLocal)
    QtRPCNetworkLocalPrivate * const d_ptr;
};

#endif // QTRPCNETWORKLOCAL_H
