#include "QtRPCNetworkTcp.h"
#include "QtRPCNetworkTcpPrivate.h"

QtRPCNetworkTcp::QtRPCNetworkTcp(int maxPeers) : QtRPCNetwork(maxPeers),
    d_ptr(new QtRPCNetworkTcpPrivate(this, maxPeers))
{

}

QtRPCNetworkTcp::~QtRPCNetworkTcp()
{
    delete d_ptr;
}

void QtRPCNetworkTcp::writeMsg(int msgId, const QByteArray &data)
{
    Q_D(QtRPCNetworkTcp);
    d->writeMsg(msgId, data);
}


