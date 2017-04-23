//
///   ConnectionType:
///       ConnectionPeer: direct connect, and max peer count is 1;
//        ConnectionPeer  RPCA <==> RPCB
//
///       ConnectionRoot: like server, has unlimited client count.
///       ConnectionBransh: like client, can connect to root just.
//                          /==>RPC5 (ConnectionBransh)
//                         ‖
//                         ‖   /==>RPCB4 (ConnectionBransh)
//                         ‖  ‖
//        ConnectionRoot  RPCA <==>RPCB1 (ConnectionBransh)
//                         ‖  ‖
//                         ‖   \==>RPCB2 (ConnectionBransh)
//                         ‖
//                          \==>RPCB3 (ConnectionBransh)
//
//
///       ConnectionDistribute:random connect, and unlimited max peer count.
//
//        ConnectionDistribute RPCA  <=====>  RPCB ConnectionDistribute
//                               |         /     
//                               |       /         
//                               |      /           RPCE ConnectionDistribute
//                               |     /           / RPCF ConnectionDistribute
//                               |    /           / /
//                               |  /            / /
//        ConnectionDistribute RPCC  <=====>  RPCD ConnectionDistribute
//
//

#ifndef QTRPCMANAGER_H
#define QTRPCMANAGER_H

#include <QObject>
#include <QUuid>
#include <typeinfo>

#define qRpc QtRPCManager::instance()

class QtRPCNetwork;
class QtRPCItemPrivate;
class Q_DECL_EXPORT QtRPCItem
{
public:
    enum SocketType
    {
        SocketUnknow = 0,
        SocketLocal, //default
        SocketTcp,
        SocketUdp,
        SocketSctp,
        SocketCustom
    };

    enum ConnectionType
    {
        ConnectionUnknow = 0,
        ConnectionPeer,  //defalut
        ConnectionRoot,
        ConnectionBransh,
        ConnectionDistribute
    };

    virtual ~QtRPCItem();
    explicit QtRPCItem(SocketType stype = SocketTcp, ConnectionType ctype = ConnectionPeer, const QString &peerId = QString());
    explicit QtRPCItem(QtRPCNetwork *network);

    bool connect(int functionId, QObject *obj, const char *slot, int timeout = -1/*future*/);
    void registerSlot(QObject *obj, const char *slot, int uniqueId, int timeout = -1/*future*/);

    SocketType socketType() const;

    QtRPCNetwork *network();
    void setNetwork(QtRPCNetwork *network);

protected:
    //msg call back.
    virtual void setMsgData(int msgId, const QByteArray &data, int descriptor = 0);

    //use to write msg.
    virtual void writeMsg(int msgid, const QByteArray &data, int descriptor = 0);

    //use to invoke.
    QObject *object(int funcId);
    const char *slotName(int funcId);

protected:
    Q_DECLARE_PRIVATE(QtRPCItem)
    QtRPCItemPrivate * const d_ptr;
    friend class QtRPCManager;
    friend class QtRPCManagerPrivate;
};

class QtRPCManagerPrivate;
class Q_DECL_EXPORT QtRPCManager
{
public:
    virtual ~QtRPCManager();
    static QtRPCManager &instance();

    //like widget's closeEvent
    void releaseBeforeEventLoopExit();

    template<typename T>
    T *call()
    {
        return dynamic_cast<T*>(item(typeid(T).name()));
    }


    template<typename T>
    T *reg()
    {
        T *item = new T();
        this->addItem(typeid(T).name(), item);
        return item;
    }

    template<typename T>
    T *reg(QtRPCItem::SocketType stype = QtRPCItem::SocketTcp,
           QtRPCItem::ConnectionType ctype = QtRPCItem::ConnectionPeer,
           const QString &peerId = QString())
    {
        T *item = new T(stype, ctype, peerId);
        this->addItem(typeid(T).name(), item);
        return item;
    }

    template<typename T1>
    T1 *reg(QtRPCNetwork *network)
    {
        T1 *item = new T1(network);
        this->addItem(typeid(T1).name(), item);
        return item;
    }

protected:
    QtRPCManager();
    QtRPCItem *item(const QString &id);
    void addItem(const QString &id, QtRPCItem *item);

private:
    friend class QtRPCItem;
    Q_DECLARE_PRIVATE(QtRPCManager)
    QtRPCManagerPrivate * const d_ptr;
};



#endif // QTRPCMANAGER_H
