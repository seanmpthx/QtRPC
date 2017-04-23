#ifndef LAN_QTRPCNETWORKTCP_H
#define LAN_QTRPCNETWORKTCP_H

#include <QUuid>
#include <QObject>
#include "../../include/QtRPCNetwork.h"

class QIODevice;
class QtRPCNetworkTcpPrivate;

class QtRPCNetworkTcp : public QtRPCNetwork
{
    Q_OBJECT
public:
    QtRPCNetworkTcp(const QString &identifier, int ctype = 1);
    ~QtRPCNetworkTcp();

    virtual void writeMsg(int msgId, const QByteArray &data, int socketId = 0);

private:
    Q_DECLARE_PRIVATE(QtRPCNetworkTcp)
    QtRPCNetworkTcpPrivate * const d_ptr;
};

#endif // LAN_QTRPCNETWORKTCP_H
