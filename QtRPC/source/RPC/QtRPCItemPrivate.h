#ifndef QTRPCITEMPRIVATE_H
#define QTRPCITEMPRIVATE_H

#include <QUuid>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QSharedPointer>

class QIODevice;
class QtRPCNetwork;
class QtRPCNetworkMgr;

class QtRPCItem;
class QtRPCItemPrivate : public QObject
{
    Q_OBJECT
public:
    struct FuncInfo
    {
        void *classPtr;
        void *funcPtr;
        QList<void*> args;
        FuncInfo() : classPtr(0), funcPtr(0){}
    };


public:
    QtRPCItemPrivate(QtRPCItem *q, int stype, int ctype, const QString &peerId);
    QtRPCItemPrivate(QtRPCItem *q, QtRPCNetwork *network);
    ~QtRPCItemPrivate();

signals:
    void doExit();
    void doGotMsg(int unique, const QByteArray &data, int socketId);

private slots:
    void onGotMsg(int unique, const QByteArray &data, int socketId);
    void writeMsg(int unique, const QByteArray &data, int socketId);

private:
    int socketType() const;

    void setMsgData(int unique, const QByteArray &data, int socketId = 0);

    void registerSlot(QObject *obj, const QByteArray &slot, int uniqueId, int timeout = -1);
    void addFunction(void *classPtr, void *functionPtr, int uniqueId, int timeout = -1);

    void callFunction(int funcId, const QList<void*> &args, void *ret);

    void create();

    QObject *object(int funcId);
    const char *slotName(int funcId);

private:
    friend class QtRPCManagerPrivate;
    Q_DECLARE_PUBLIC(QtRPCItem)
    QtRPCItem * const q_ptr;

private:
    const int socketType_;
    const int connectiontype_;
    const QString peerId_;

    QSharedPointer<QtRPCNetwork> networkItem;
    QMap<int, QPair<void*, void*> > functions_;
    QMap<int, QPair<QObject*, QByteArray> > slotsCache;
};

#endif // QTRPCITEMPRIVATE_H
