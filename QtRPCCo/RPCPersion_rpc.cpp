//版权所有 Xairy© Co., Ltd.
// ==================depend headers==================
#include "RPCPersion.h"


// ================== function ids  ==================
const QPair<int, QString> rpc_functoin_testfunc(1, "QStringList RPCPersion::testfunc(QStringList & list1, QStringList & list2, MyInfo * x)");
const QPair<int, QString> rpc_functoin_setinfo(2, "void RPCPersion::setInfo(const MyInfo & info1, const MyInfoX & info2)");
const QPair<int, QString> rpc_functoin_settext(3, "void RPCPersion::setText(const QString & text)");
const QPair<int, QString> rpc_functoin_getname(4, "QString * RPCPersion::getName()");
const QPair<int, QString> rpc_functoin_getname2(5, "QMap < QString , QList < UnKonwType > > * RPCPersion::getName2()");
const QPair<int, QString> rpc_functoin_getname1(6, "const QMap < QString , QList < UnKonwType > > * RPCPersion::getName1()");



// ================operator functions================
QDataStream &operator <<(QDataStream &stream, const UnKonwType &unkonwtype_)
{
    stream  << (unkonwtype_.a == 0 ? QString() : *unkonwtype_.a)
    << unkonwtype_.b << unkonwtype_.c << unkonwtype_.info;
    return stream;
}
QDataStream &operator <<(QDataStream &stream, UnKonwType &unkonwtype_)
{
    stream  >> (unkonwtype_.a == 0 ? QString() : *unkonwtype_.a)
    >> unkonwtype_.b >> unkonwtype_.c >> unkonwtype_.info;
    return stream;
}


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
#include "RPCPersion_rpc.moc"                         

// ===================rpc functions===================
QStringList RPCPersion::testfunc(QStringList & list1, QStringList & list2, MyInfo * x)
{
    QByteArray rpc_arg_data;
    QDataStream rpc_stream_arg(&rpc_arg_data, QIODevice::WriteOnly);
    rpc_stream_arg.setVersion(QDataStream::Qt_4_8);
    rpc_stream_arg << list1 << list2 << *x;
    writeMsg(rpc_functoin_testfunc.first, rpc_arg_data);

    RpcEventLoop loop(rpc_functoin_testfunc.first);
    QObject::connect(d_ptr, SIGNAL(gotMsg(int,QByteArray)),
                 &loop, SLOT(rpcExit(int,QByteArray)));
    loop.exec();
    QStringList  rpc_temp_ret ;
    const QByteArray &rpc_ret_data = loop.data();
    QDataStream rpc_stream_ret(rpc_ret_data);
    rpc_stream_ret.setVersion(QDataStream::Qt_4_8);
    rpc_stream_ret >> rpc_temp_ret;
    return rpc_temp_ret;
}

void RPCPersion::setInfo(const MyInfo & info1, const MyInfoX & info2)
{
    //write
    QByteArray rpc_arg_data;
    QDataStream rpc_stream_arg(&rpc_arg_data, QIODevice::WriteOnly);
    rpc_stream_arg.setVersion(QDataStream::Qt_4_8);
    rpc_stream_arg << info1 << info2;
    writeMsg(rpc_functoin_setinfo.first, rpc_arg_data);
}

void RPCPersion::setText(const QString & text)
{
    //write
    QByteArray rpc_arg_data;
    QDataStream rpc_stream_arg(&rpc_arg_data, QIODevice::WriteOnly);
    rpc_stream_arg.setVersion(QDataStream::Qt_4_8);
    rpc_stream_arg << text;
    writeMsg(rpc_functoin_settext.first, rpc_arg_data);
}

QString * RPCPersion::getName()
{
    
    RpcEventLoop loop(rpc_functoin_getname.first);
    QObject::connect(d_ptr, SIGNAL(gotMsg(int,QByteArray)),
                 &loop, SLOT(rpcExit(int,QByteArray)));
    loop.exec();
    QString * rpc_temp_ret = new QString();
    const QByteArray &rpc_ret_data = loop.data();
    QDataStream rpc_stream_ret(rpc_ret_data);
    rpc_stream_ret.setVersion(QDataStream::Qt_4_8);
    rpc_stream_ret >> *rpc_temp_ret;
    return rpc_temp_ret;
}

QMap < QString , QList < UnKonwType > > * RPCPersion::getName2()
{
    
    RpcEventLoop loop(rpc_functoin_getname2.first);
    QObject::connect(d_ptr, SIGNAL(gotMsg(int,QByteArray)),
                 &loop, SLOT(rpcExit(int,QByteArray)));
    loop.exec();
    QMap < QString , QList < UnKonwType > > * rpc_temp_ret = new QMap < QString , QList < UnKonwType > >();
    const QByteArray &rpc_ret_data = loop.data();
    QDataStream rpc_stream_ret(rpc_ret_data);
    rpc_stream_ret.setVersion(QDataStream::Qt_4_8);
    rpc_stream_ret >> *rpc_temp_ret;
    return rpc_temp_ret;
}

const QMap < QString , QList < UnKonwType > > * RPCPersion::getName1()
{
    
    RpcEventLoop loop(rpc_functoin_getname1.first);
    QObject::connect(d_ptr, SIGNAL(gotMsg(int,QByteArray)),
                 &loop, SLOT(rpcExit(int,QByteArray)));
    loop.exec();
    QMap < QString , QList < UnKonwType > > * rpc_temp_ret = new QMap < QString , QList < UnKonwType > >();
    const QByteArray &rpc_ret_data = loop.data();
    QDataStream rpc_stream_ret(rpc_ret_data);
    rpc_stream_ret.setVersion(QDataStream::Qt_4_8);
    rpc_stream_ret >> *rpc_temp_ret;
    return rpc_temp_ret;
}

