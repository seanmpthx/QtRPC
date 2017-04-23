#ifndef LAN_QTRPCNETWORKLOCAL_H
#define LAN_QTRPCNETWORKLOCAL_H

#include <QUuid>
#include <QObject>
#include "../../include/QtRPCNetwork.h"

class QIODevice;
class QtRPCNetworkLocalPrivate;

class QtRPCNetworkLocal : public QtRPCNetwork
{
    Q_OBJECT
public:
    QtRPCNetworkLocal(const QString &identifier, int ctype);
    ~QtRPCNetworkLocal();

    virtual void writeMsg(int msgId, const QByteArray &data, int socketId = 0);

private:
    Q_DECLARE_PRIVATE(QtRPCNetworkLocal)
    QtRPCNetworkLocalPrivate * const d_ptr;
};

#endif // LAN_QTRPCNETWORKLOCAL_H
