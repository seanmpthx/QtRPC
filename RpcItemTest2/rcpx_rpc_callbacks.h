#ifndef RCPX_RPC_CALLBACKS_H
#define RCPX_RPC_CALLBACKS_H

#include "QtRPCManager.h"

const int rpc_functoin_testfunction9 = 9;//DataTest2 MyRPC::testFunction9()
const int rpc_functoin_testfunction8 = 8;//void MyRPC::testFunction8(const DataTest1 & data)
const int rpc_functoin_testfunction7 = 7;//void MyRPC::testFunction7(const QByteArray & data)
const int rpc_functoin_testfunction6 = 6;//QMap < QString , QPair < QString , QString > > MyRPC::testFunction6(const QString & conf)
const int rpc_functoin_testfunction5 = 5;//QString * MyRPC::testFunction5(QString * arg1)
const int rpc_functoin_testfunction4 = 4;//void MyRPC::testFunction4(QString * arg1)
const int rpc_functoin_testfunction3 = 3;//QString MyRPC::testFunction3(const QString & arg1, const QString & arg2)
const int rpc_functoin_testfunction2 = 2;//QString MyRPC::testFunction2()
const int rpc_functoin_testfunction1 = 1;//void MyRPC::TestFunction1(int type)

#include "rpc_types.h"
#include <QMap>
#include <QString>
#include <QPair>
#include <QDataStream>
static QDataStream &operator <<(QDataStream &stream, const DataTest2 &datatest2_)
{
    stream  << datatest2_.arg1;
    return stream;
}
static QDataStream &operator >>(QDataStream &stream, DataTest2 &datatest2_)
{
    stream  >> datatest2_.arg1;
    return stream;
}

static QDataStream &operator <<(QDataStream &stream, const DataTest1 &datatest1_)
{
    stream  << datatest1_.arg1 << datatest1_.arg2 << (qint8)datatest1_.arg3;
    return stream;
}
static QDataStream &operator >>(QDataStream &stream, DataTest1 &datatest1_)
{
    stream  >> datatest1_.arg1 >> datatest1_.arg2 >> *(qint8*)(&datatest1_.arg3);
    return stream;
}


class MyRPC : public QtRPCItem
{
public:
    explicit MyRPC(SocketType stype = SocketTcp, ConnectionType ctype = ConnectionPeer,
                      const QString &peerId = QString()) : QtRPCItem(stype, ctype, peerId){}
    explicit MyRPC(QtRPCNetwork *network) : QtRPCItem(network){}

    virtual void setMsgData(int unique, const QByteArray &data_rpc_in, int descriptor = 0)
    {
        QDataStream rpc_stream_arg(data_rpc_in);
        rpc_stream_arg.setVersion(QDataStream::Qt_4_8);
        switch(unique)
        {
        case rpc_functoin_testfunction9:
        {
            
            
            QList<void*> void_args;
            
            DataTest2 rpc_temp_ret ;
            QMetaObject::invokeMethod(this->object(rpc_functoin_testfunction9),this->slotName(rpc_functoin_testfunction9),QReturnArgument<DataTest2 >("DataTest2", rpc_temp_ret));
            QByteArray data_send_back;
            QDataStream stream_send_back(&data_send_back, QIODevice::WriteOnly);
            stream_send_back << rpc_temp_ret;
            writeMsg(unique, data_send_back, descriptor);

            break;
        }
        case rpc_functoin_testfunction8:
        {
            DataTest1 data;
            rpc_stream_arg >> data;

            QList<void*> void_args;
            void_args << &data;

        
            QMetaObject::invokeMethod(this->object(rpc_functoin_testfunction8),this->slotName(rpc_functoin_testfunction8),QArgument<DataTest1 >("DataTest1", data));
            
            break;
        }
        case rpc_functoin_testfunction7:
        {
            QByteArray data;
            rpc_stream_arg >> data;

            QList<void*> void_args;
            void_args << &data;

        
            QMetaObject::invokeMethod(this->object(rpc_functoin_testfunction7),this->slotName(rpc_functoin_testfunction7),QArgument<QByteArray >("QByteArray", data));
            
            break;
        }
        case rpc_functoin_testfunction6:
        {
            QString conf;
            rpc_stream_arg >> conf;

            QList<void*> void_args;
            void_args << &conf;

            QMap < QString , QPair < QString , QString > > rpc_temp_ret ;
            QMetaObject::invokeMethod(this->object(rpc_functoin_testfunction6),this->slotName(rpc_functoin_testfunction6),QReturnArgument<QMap < QString , QPair < QString , QString > > >("QMap < QString , QPair < QString , QString > >", rpc_temp_ret),QArgument<QString >("QString", conf));
            QByteArray data_send_back;
            QDataStream stream_send_back(&data_send_back, QIODevice::WriteOnly);
            stream_send_back << rpc_temp_ret;
            writeMsg(unique, data_send_back, descriptor);

            break;
        }
        case rpc_functoin_testfunction5:
        {
            QString arg1;
            rpc_stream_arg >> arg1;

            QList<void*> void_args;
            void_args << &arg1;

            QString * rpc_temp_ret = new QString();
            QMetaObject::invokeMethod(this->object(rpc_functoin_testfunction5),this->slotName(rpc_functoin_testfunction5),QReturnArgument<QString >("QString", *rpc_temp_ret),QArgument<QString >("QString", arg1));
            QByteArray data_send_back;
            QDataStream stream_send_back(&data_send_back, QIODevice::WriteOnly);
            stream_send_back << *rpc_temp_ret;
            writeMsg(unique, data_send_back, descriptor);

            break;
        }
        case rpc_functoin_testfunction4:
        {
            QString arg1;
            rpc_stream_arg >> arg1;

            QList<void*> void_args;
            void_args << &arg1;

        
            QMetaObject::invokeMethod(this->object(rpc_functoin_testfunction4),this->slotName(rpc_functoin_testfunction4),QArgument<QString >("QString", arg1));
            
            break;
        }
        case rpc_functoin_testfunction3:
        {
            QString arg1;QString arg2;
            rpc_stream_arg >> arg1 >> arg2;

            QList<void*> void_args;
            void_args << &arg1 << &arg2;

            QString rpc_temp_ret ;
            QMetaObject::invokeMethod(this->object(rpc_functoin_testfunction3),this->slotName(rpc_functoin_testfunction3),QReturnArgument<QString >("QString", rpc_temp_ret),QArgument<QString >("QString", arg1),QArgument<QString >("QString", arg2));
            QByteArray data_send_back;
            QDataStream stream_send_back(&data_send_back, QIODevice::WriteOnly);
            stream_send_back << rpc_temp_ret;
            writeMsg(unique, data_send_back, descriptor);

            break;
        }
        case rpc_functoin_testfunction2:
        {
            
            
            QList<void*> void_args;
            
            QString rpc_temp_ret ;
            QMetaObject::invokeMethod(this->object(rpc_functoin_testfunction2),this->slotName(rpc_functoin_testfunction2),QReturnArgument<QString >("QString", rpc_temp_ret));
            QByteArray data_send_back;
            QDataStream stream_send_back(&data_send_back, QIODevice::WriteOnly);
            stream_send_back << rpc_temp_ret;
            writeMsg(unique, data_send_back, descriptor);

            break;
        }
        case rpc_functoin_testfunction1:
        {
            int type;
            rpc_stream_arg >> type;

            QList<void*> void_args;
            void_args << &type;

        
            QMetaObject::invokeMethod(this->object(rpc_functoin_testfunction1),this->slotName(rpc_functoin_testfunction1),QArgument<int >("int", type));
            
            break;
        }
        }
    }
};

#endif //RCPX_RPC_CALLBACKS_H