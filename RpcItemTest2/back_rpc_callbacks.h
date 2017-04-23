#ifndef BACK_RPC_CALLBACKS_H
#define BACK_RPC_CALLBACKS_H

#include "QtRPCManager.h"

const int rpc_functoin_testfunction2 = 2;//QString Back::testFunction2()
const int rpc_functoin_testfunction1 = 1;//void Back::TestFunction1()

#include <QString>
#include <QDataStream>
class Back : public QtRPCItem
{
public:
    Back(QtRPCManager *mgr, int id, int stype, int rtype) : QtRPCItem(mgr, id, stype, rtype){}

    virtual void setMsgData(int unique, const QByteArray &data_rpc_in)
    {
        QDataStream rpc_stream_arg(data_rpc_in);
        rpc_stream_arg.setVersion(QDataStream::Qt_4_8);
        switch(unique)
        {
        case rpc_functoin_testfunction2:
        {
            
            
            QList<void*> void_args;
            
            QString rpc_temp_ret ;
            QMetaObject::invokeMethod(this->object(rpc_functoin_testfunction2),this->slotName(rpc_functoin_testfunction2),QReturnArgument<QString >("QString", rpc_temp_ret));
            QByteArray data_send_back;
            QDataStream stream_send_back(&data_send_back, QIODevice::WriteOnly);
            stream_send_back << rpc_temp_ret;
            writeMsg(unique, data_send_back);

            break;
        }
        case rpc_functoin_testfunction1:
        {
            
            
            QList<void*> void_args;
            
        
            QMetaObject::invokeMethod(this->object(rpc_functoin_testfunction1),this->slotName(rpc_functoin_testfunction1));
            
            break;
        }
        }
    }
};

#endif //BACK_RPC_CALLBACKS_H