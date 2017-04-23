//版权所有 Xairy© Co., Ltd.
// ==================depend headers==================
#include "rcpx.h"


// ================== function ids  ==================
const QPair<int, QString> rpc_functoin_testfunction2(1, "QString MyRPC::testFunction2()");
const QPair<int, QString> rpc_functoin_testfunction1(2, "void MyRPC::TestFunction1()");



// ================operator functions================

#include <QEventLoop>                           
class RpcEventLoop : public QEventLoop          
{                                               
    Q_OBJECT                                    
public:                                         
    RpcEventLoop(int id) : id_(id){}            
    ~RpcEventLoop() {}                          
    const QByteArray &data() const              
    {                                           
        return data_;                           
    }                                           
                                                
public slots:                                   
    void rpcExit(int id, const QByteArray &data)
    {                                           
        if (id != id_)                          
            return;                             
        data_ = data;                           
        QEventLoop::exit();                     
    }                                           
                                                
private:                                        
    int id_;                                    
    QByteArray data_;                           
};                                              
#include "rcpx_rpc.moc"                         

// ===================rpc functions===================
QString MyRPC::testFunction2()
{
    QByteArray rpc_arg_data;
    QDataStream rpc_stream_arg(&rpc_arg_data, QIODevice::WriteOnly);
    rpc_stream_arg.setVersion(QDataStream::Qt_4_8);
    rpc_stream_arg << rpc_functoin_testfunction2.first ;
    writeMsg(rpc_functoin_testfunction2.first, rpc_arg_data);

    RpcEventLoop loop(rpc_functoin_testfunction2.first);
    QObject::connect(d_ptr, SIGNAL(gotMsg(int,QByteArray)),
                 &loop, SLOT(rpcExit(int,QByteArray)));
    loop.exec();
    QString  rpc_temp_ret ;
    const QByteArray &rpc_ret_data = loop.data();
    QDataStream rpc_stream_ret(rpc_ret_data);
    rpc_stream_ret.setVersion(QDataStream::Qt_4_8);
    rpc_stream_ret >> rpc_temp_ret;
    return rpc_temp_ret;
}

void MyRPC::TestFunction1()
{
    //write
    QByteArray rpc_arg_data;
    QDataStream rpc_stream_arg(&rpc_arg_data, QIODevice::WriteOnly);
    rpc_stream_arg.setVersion(QDataStream::Qt_4_8);
    rpc_stream_arg << rpc_functoin_testfunction1.first ;
    writeMsg(rpc_functoin_testfunction1.first, rpc_arg_data);
}

