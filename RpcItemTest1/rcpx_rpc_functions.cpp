//版权所有 Xairy© Co., Ltd.
// ==================depend headers==================
#include "rcpx.h"
#include "rpc_types.h"
#include <QMap>
#include <QString>
#include <QPair>
#include <QDataStream>


// ================== function ids  ==================

const int rpc_functoin_testfunction9 = 9;//DataTest2 MyRPC::testFunction9()
const int rpc_functoin_testfunction8 = 8;//void MyRPC::testFunction8(const DataTest1 & data)
const int rpc_functoin_testfunction7 = 7;//void MyRPC::testFunction7(const QByteArray & data)
const int rpc_functoin_testfunction6 = 6;//QMap < QString , QPair < QString , QString > > MyRPC::testFunction6(const QString & conf)
const int rpc_functoin_testfunction5 = 5;//QString * MyRPC::testFunction5(QString * arg1)
const int rpc_functoin_testfunction4 = 4;//void MyRPC::testFunction4(QString * arg1)
const int rpc_functoin_testfunction3 = 3;//QString MyRPC::testFunction3(const QString & arg1, const QString & arg2)
const int rpc_functoin_testfunction2 = 2;//QString MyRPC::testFunction2()
const int rpc_functoin_testfunction1 = 1;//void MyRPC::TestFunction1(int type)



// ================operator functions================
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


#include <QEventLoop>                                               
class RpcEventLoop : public QEventLoop                              
{                                                                   
    Q_OBJECT                                                        
public:                                                             
    RpcEventLoop(int id) : id_(id), descriptor_(0){}                
    ~RpcEventLoop() {}                                              
    const QByteArray &data()                                        
    {                                                               
        this->exec();                                               
        return data_;                                               
    }                                                               
    int descriptor()                                                
    {                                                               
        return descriptor_;                                         
    }                                                               
                                                                    
public slots:                                                       
    void rpcExit(int msgId, const QByteArray &data, int descriptor) 
    {                                                               
        if (msgId != id_)                                           
            return;                                                 
        data_ = data;                                               
        descriptor_ = descriptor;                                   
        QEventLoop::exit();                                         
    }                                                               
                                                                    
private:                                                            
    int id_;                                                        
    int descriptor_;                                                
    QByteArray data_;                                               
};                                                                  
#include "rcpx_rpc_functions.moc"                                   

// ===================rpc functions===================
DataTest2 MyRPC::testFunction9()
{
    QByteArray rpc_arg_data;
    QDataStream rpc_stream_arg(&rpc_arg_data, QIODevice::WriteOnly);
    rpc_stream_arg.setVersion(QDataStream::Qt_4_8);
    rpc_stream_arg;
    RpcEventLoop loop(rpc_functoin_testfunction9);
    QObject::connect((QObject*)d_ptr, SIGNAL(doGotMsg(int,QByteArray,int)),
                     &loop, SLOT(rpcExit(int,QByteArray,int)));
    QObject::connect((QObject*)d_ptr, SIGNAL(doExit()),
                     &loop, SLOT(quit()));
    writeMsg(rpc_functoin_testfunction9, rpc_arg_data, loop.descriptor());
        DataTest2 rpc_temp_ret ;
    const QByteArray &rpc_ret_data = loop.data();
    QDataStream rpc_stream_ret(rpc_ret_data);
    rpc_stream_ret.setVersion(QDataStream::Qt_4_8);
    rpc_stream_ret >> rpc_temp_ret;
    return rpc_temp_ret;
}

void MyRPC::testFunction8(const DataTest1 & data)
{
    QByteArray rpc_arg_data;
    QDataStream rpc_stream_arg(&rpc_arg_data, QIODevice::WriteOnly);
    rpc_stream_arg.setVersion(QDataStream::Qt_4_8);
    rpc_stream_arg << data;
    RpcEventLoop loop(rpc_functoin_testfunction8);
    QObject::connect((QObject*)d_ptr, SIGNAL(doGotMsg(int,QByteArray,int)),
                     &loop, SLOT(rpcExit(int,QByteArray,int)));
    QObject::connect((QObject*)d_ptr, SIGNAL(doExit()),
                     &loop, SLOT(quit()));
    writeMsg(rpc_functoin_testfunction8, rpc_arg_data, loop.descriptor());
}

void MyRPC::testFunction7(const QByteArray & data)
{
    QByteArray rpc_arg_data;
    QDataStream rpc_stream_arg(&rpc_arg_data, QIODevice::WriteOnly);
    rpc_stream_arg.setVersion(QDataStream::Qt_4_8);
    rpc_stream_arg << data;
    RpcEventLoop loop(rpc_functoin_testfunction7);
    QObject::connect((QObject*)d_ptr, SIGNAL(doGotMsg(int,QByteArray,int)),
                     &loop, SLOT(rpcExit(int,QByteArray,int)));
    QObject::connect((QObject*)d_ptr, SIGNAL(doExit()),
                     &loop, SLOT(quit()));
    writeMsg(rpc_functoin_testfunction7, rpc_arg_data, loop.descriptor());
}

QMap < QString , QPair < QString , QString > > MyRPC::testFunction6(const QString & conf)
{
    QByteArray rpc_arg_data;
    QDataStream rpc_stream_arg(&rpc_arg_data, QIODevice::WriteOnly);
    rpc_stream_arg.setVersion(QDataStream::Qt_4_8);
    rpc_stream_arg << conf;
    RpcEventLoop loop(rpc_functoin_testfunction6);
    QObject::connect((QObject*)d_ptr, SIGNAL(doGotMsg(int,QByteArray,int)),
                     &loop, SLOT(rpcExit(int,QByteArray,int)));
    QObject::connect((QObject*)d_ptr, SIGNAL(doExit()),
                     &loop, SLOT(quit()));
    writeMsg(rpc_functoin_testfunction6, rpc_arg_data, loop.descriptor());
        QMap < QString , QPair < QString , QString > > rpc_temp_ret ;
    const QByteArray &rpc_ret_data = loop.data();
    QDataStream rpc_stream_ret(rpc_ret_data);
    rpc_stream_ret.setVersion(QDataStream::Qt_4_8);
    rpc_stream_ret >> rpc_temp_ret;
    return rpc_temp_ret;
}

QString * MyRPC::testFunction5(QString * arg1)
{
    QByteArray rpc_arg_data;
    QDataStream rpc_stream_arg(&rpc_arg_data, QIODevice::WriteOnly);
    rpc_stream_arg.setVersion(QDataStream::Qt_4_8);
    rpc_stream_arg << *arg1;
    RpcEventLoop loop(rpc_functoin_testfunction5);
    QObject::connect((QObject*)d_ptr, SIGNAL(doGotMsg(int,QByteArray,int)),
                     &loop, SLOT(rpcExit(int,QByteArray,int)));
    QObject::connect((QObject*)d_ptr, SIGNAL(doExit()),
                     &loop, SLOT(quit()));
    writeMsg(rpc_functoin_testfunction5, rpc_arg_data, loop.descriptor());
        QString * rpc_temp_ret = new QString();
    const QByteArray &rpc_ret_data = loop.data();
    QDataStream rpc_stream_ret(rpc_ret_data);
    rpc_stream_ret.setVersion(QDataStream::Qt_4_8);
    rpc_stream_ret >> *rpc_temp_ret;
    return rpc_temp_ret;
}

void MyRPC::testFunction4(QString * arg1)
{
    QByteArray rpc_arg_data;
    QDataStream rpc_stream_arg(&rpc_arg_data, QIODevice::WriteOnly);
    rpc_stream_arg.setVersion(QDataStream::Qt_4_8);
    rpc_stream_arg << *arg1;
    RpcEventLoop loop(rpc_functoin_testfunction4);
    QObject::connect((QObject*)d_ptr, SIGNAL(doGotMsg(int,QByteArray,int)),
                     &loop, SLOT(rpcExit(int,QByteArray,int)));
    QObject::connect((QObject*)d_ptr, SIGNAL(doExit()),
                     &loop, SLOT(quit()));
    writeMsg(rpc_functoin_testfunction4, rpc_arg_data, loop.descriptor());
}

QString MyRPC::testFunction3(const QString & arg1, const QString & arg2)
{
    QByteArray rpc_arg_data;
    QDataStream rpc_stream_arg(&rpc_arg_data, QIODevice::WriteOnly);
    rpc_stream_arg.setVersion(QDataStream::Qt_4_8);
    rpc_stream_arg << arg1 << arg2;
    RpcEventLoop loop(rpc_functoin_testfunction3);
    QObject::connect((QObject*)d_ptr, SIGNAL(doGotMsg(int,QByteArray,int)),
                     &loop, SLOT(rpcExit(int,QByteArray,int)));
    QObject::connect((QObject*)d_ptr, SIGNAL(doExit()),
                     &loop, SLOT(quit()));
    writeMsg(rpc_functoin_testfunction3, rpc_arg_data, loop.descriptor());
        QString rpc_temp_ret ;
    const QByteArray &rpc_ret_data = loop.data();
    QDataStream rpc_stream_ret(rpc_ret_data);
    rpc_stream_ret.setVersion(QDataStream::Qt_4_8);
    rpc_stream_ret >> rpc_temp_ret;
    return rpc_temp_ret;
}

QString MyRPC::testFunction2()
{
    QByteArray rpc_arg_data;
    QDataStream rpc_stream_arg(&rpc_arg_data, QIODevice::WriteOnly);
    rpc_stream_arg.setVersion(QDataStream::Qt_4_8);
    rpc_stream_arg;
    RpcEventLoop loop(rpc_functoin_testfunction2);
    QObject::connect((QObject*)d_ptr, SIGNAL(doGotMsg(int,QByteArray,int)),
                     &loop, SLOT(rpcExit(int,QByteArray,int)));
    QObject::connect((QObject*)d_ptr, SIGNAL(doExit()),
                     &loop, SLOT(quit()));
    writeMsg(rpc_functoin_testfunction2, rpc_arg_data, loop.descriptor());
        QString rpc_temp_ret ;
    const QByteArray &rpc_ret_data = loop.data();
    QDataStream rpc_stream_ret(rpc_ret_data);
    rpc_stream_ret.setVersion(QDataStream::Qt_4_8);
    rpc_stream_ret >> rpc_temp_ret;
    return rpc_temp_ret;
}

void MyRPC::TestFunction1(int type)
{
    QByteArray rpc_arg_data;
    QDataStream rpc_stream_arg(&rpc_arg_data, QIODevice::WriteOnly);
    rpc_stream_arg.setVersion(QDataStream::Qt_4_8);
    rpc_stream_arg << type;
    RpcEventLoop loop(rpc_functoin_testfunction1);
    QObject::connect((QObject*)d_ptr, SIGNAL(doGotMsg(int,QByteArray,int)),
                     &loop, SLOT(rpcExit(int,QByteArray,int)));
    QObject::connect((QObject*)d_ptr, SIGNAL(doExit()),
                     &loop, SLOT(quit()));
    writeMsg(rpc_functoin_testfunction1, rpc_arg_data, loop.descriptor());
}

