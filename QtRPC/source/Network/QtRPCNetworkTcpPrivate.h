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

class QtRPCNetworkTcp;
class QtRPCNetworkTcpPrivate : public QObject
{
    Q_OBJECT
public:
    QtRPCNetworkTcpPrivate(QtRPCNetworkTcp *q, int maxPeers);
    ~QtRPCNetworkTcpPrivate();

private:
    void writeMsg(int msgid, const QByteArray &data);

private slots:
    void newConnection();
    void readReady();
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);

    void recreateConnection();

    void onGotPeer(const QString &peer, const QString &ip, quint16 port);

private:
    void createClient();
    void createServer();
    void clearConnections();
    void connectToServer(const QString &address, quint16 port);
    void connectSignals(QObject *obj);

    void writeSections(int id, const QByteArray &data);

private:
    Q_DECLARE_PUBLIC(QtRPCNetworkTcp)
    QtRPCNetworkTcp * const q_ptr;

private:
    quint16 myPort_;

    MsgSection *sectionCtrl_;

    QSharedPointer<QTcpServer> tcpServer_;
    QSharedPointer<QTcpSocket> tcpSocketOut_;
    QTcpSocket *tcpSocketIn_;

    LanSearcher searcher_;
};

#endif // QTRPCNETWORKTCPPRIVATE_H
