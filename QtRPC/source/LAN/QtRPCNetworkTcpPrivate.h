#ifndef LAN_QTRPCNETWORKTCPPRIVATE_H
#define LAN_QTRPCNETWORKTCPPRIVATE_H

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
    QtRPCNetworkTcpPrivate(QtRPCNetworkTcp *q, const QString &identifier, int ctype);
    ~QtRPCNetworkTcpPrivate();

private:
    void writeMsg(int msgid, const QByteArray &data, int socketId);

private slots:
    void newConnection();
    void readReady();
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);

    void recreateConnection(QTcpSocket *socket = 0);

    void onGotPeer(const QString &peer, const QString &ip, quint16 port);
    void onGotMsg(int msgid, const QByteArray &data, int socketId);

private:
    void createClient();
    void createServer();
    void clearConnections(QTcpSocket *socket = 0);
    void connectToServer(const QString &address, quint16 port);
    void connectSignals(QObject *obj);

    void writeSections(int id, const QByteArray &data, int mySocketId);

private:
    Q_DECLARE_PUBLIC(QtRPCNetworkTcp)
    QtRPCNetworkTcp * const q_ptr;

private:
    const QString identifier_;
    quint16 myPort_;

    QMap<QTcpSocket*, MsgSection*> sectionCtrls_;

    QSharedPointer<QTcpServer> tcpServer_;
    QSharedPointer<QTcpSocket> tcpSocketOut_;

    LanSearcher searcher_;

    int connectionType;

    int currentPaddings;

    bool exitMark;
};

#endif // LAN_QTRPCNETWORKTCPPRIVATE_H
