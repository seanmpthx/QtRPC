#include "QtRPCNetworkLocal.h"
#include "QtRPCNetworkLocalPrivate.h"

QtRPCNetworkLocal::QtRPCNetworkLocal(const QString &identifier, int ctype) : QtRPCNetwork(ctype),
    d_ptr(new QtRPCNetworkLocalPrivate(this, identifier, ctype))
{

}

QtRPCNetworkLocal::~QtRPCNetworkLocal()
{
    delete d_ptr;
}

void QtRPCNetworkLocal::writeMsg(int msgId, const QByteArray &data, int socketId)
{
    Q_D(QtRPCNetworkLocal);
    d->writeMsg(msgId, data, socketId);
}


