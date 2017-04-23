#include "QtRPCNetworkTcp.h"
#include "QtRPCNetworkTcpPrivate.h"

QtRPCNetworkTcp::QtRPCNetworkTcp(const QString &identifier, int ctype) : QtRPCNetwork(ctype),
    d_ptr(new QtRPCNetworkTcpPrivate(this, identifier, ctype))
{

}

QtRPCNetworkTcp::~QtRPCNetworkTcp()
{
    delete d_ptr;
}

void QtRPCNetworkTcp::writeMsg(int msgId, const QByteArray &data, int socketId)
{
    Q_D(QtRPCNetworkTcp);
    d->writeMsg(msgId, data, socketId);
}


