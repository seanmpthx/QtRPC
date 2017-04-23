#include "QtRPCNetworkLocal.h"
#include "QtRPCNetworkLocalPrivate.h"

QtRPCNetworkLocal::QtRPCNetworkLocal(int maxPeers) : QtRPCNetwork(maxPeers),
    d_ptr(new QtRPCNetworkLocalPrivate(this, maxPeers))
{

}

QtRPCNetworkLocal::~QtRPCNetworkLocal()
{
    delete d_ptr;
}

void QtRPCNetworkLocal::writeMsg(int msgId, const QByteArray &data)
{
    Q_D(QtRPCNetworkLocal);
    d->writeMsg(msgId, data);
}


