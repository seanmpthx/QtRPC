#ifndef QTRPCNETWORKTCPPRIVATE_H
#define QTRPCNETWORKTCPPRIVATE_H

#include <QUuid>
#include <QTimer>
#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>
#include <QDataStream>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSharedPointer>
#include "MsgSection.h"
#include "LanSearcher.h"

class QtRPCNetworkLocal;
class QtRPCNetworkLocalPrivate : public QObject
{
    Q_OBJECT
public:
    QtRPCNetworkLocalPrivate(QtRPCNetworkLocal *q, int maxPeers);
    ~QtRPCNetworkLocalPrivate();

private:
    void writeMsg(int msgid, const QByteArray &data);

private slots:
    void newConnection();
    void readReady();
    void connected();
    void disconnected();
    void error(QLocalSocket::LocalSocketError socketError);

    void recreateConnection();

    void onGotPeer(const QString &peer, const QString &ip, quint16 port);

private:
    void createClient();
    void createServer();
    void clearConnections();
    void connectToServer(const QString &address);
    void connectSignals(QObject *obj);

    void writeSections(int id, const QByteArray &data);

private:
    Q_DECLARE_PUBLIC(QtRPCNetworkLocal)
    QtRPCNetworkLocal * const q_ptr;

private:
    const QString peerAddress_;

    MsgSection *sectionCtrl_;

    QSharedPointer<QLocalServer> localServer_;
    QSharedPointer<QLocalSocket> localSocketOut_;
    QLocalSocket *localSocketIn_;

    LanSearcher searcher_;
};

#endif // QTRPCNETWORKTCPPRIVATE_H
