#ifndef QTRPCNETWORK
#define QTRPCNETWORK

#include <QObject>

class QtRPCNetwork : public QObject
{
    Q_OBJECT
public:
    QtRPCNetwork(int /*ctype*/ = 1){}
    virtual ~QtRPCNetwork(){}

    virtual void writeMsg(int msgId, const QByteArray &data, int socketId = 0) = 0;

signals:
    void doGotMsg(int msgId, const QByteArray &data, int socketId = 0);
};


#endif // QTRPCNETWORK

