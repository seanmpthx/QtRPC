#ifndef RCPX
#define RCPX

#include "QtRPCManager.h"
#include <QMap>
#include <QPair>
#include "rpc_types.h"


class MyRPC : public QtRPCItem
{
public:
    explicit MyRPC(SocketType stype = SocketTcp, ConnectionType ctype = ConnectionPeer, const QString &identifier = QString()) : QtRPCItem(stype, ctype, identifier){}
    explicit MyRPC(QtRPCNetwork *network) : QtRPCItem(network){}

    void TestFunction1(int type);
    QString testFunction2();
    QString testFunction3(const QString &arg1, const QString &arg2);
    void testFunction4(QString *arg1);
    QString *testFunction5(QString *arg1);
    QMap<QString, QPair<QString, QString> > testFunction6(const QString &conf);
    void testFunction7(const QByteArray &data);
    void testFunction8(const DataTest1 &data);
    DataTest2 testFunction9();

};

#endif // RCPX

