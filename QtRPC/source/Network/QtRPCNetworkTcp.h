#ifndef QTRPCNETWORKTCP_H
#define QTRPCNETWORKTCP_H

#include <QUuid>
#include <QObject>
#include "../../include/QtRPCNetwork.h"

class QIODevice;
class QtRPCNetworkTcpPrivate;

class QtRPCNetworkTcp : public QtRPCNetwork
{
    Q_OBJECT
public:
    QtRPCNetworkTcp(int maxPeers);
    ~QtRPCNetworkTcp();

    virtual void writeMsg(int msgId, const QByteArray &data);

private:
    Q_DECLARE_PRIVATE(QtRPCNetworkTcp)
    QtRPCNetworkTcpPrivate * const d_ptr;
};

#endif // QTRPCNETWORKTCP_H
