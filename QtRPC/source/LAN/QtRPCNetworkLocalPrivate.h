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

class QtRPCNetworkLocal;
class QtRPCNetworkLocalPrivate : public QObject
{
    Q_OBJECT
public:
    QtRPCNetworkLocalPrivate(QtRPCNetworkLocal *q, const QString &identifier, int ctype);
    ~QtRPCNetworkLocalPrivate();

private:
    void writeMsg(int msgid, const QByteArray &data, int socketId = 0);

private slots:
    void newConnection();
    void readReady();
    void connected();
    void disconnected();
    void error(QLocalSocket::LocalSocketError socketError);

    void recreateConnection(QLocalSocket *socket = 0);

    void onGotPeer(const QString &peer, const QString &ip, quint16 port);
    void onGotMsg(int msgid, const QByteArray &data, int socketId);

private:
    void createClient();
    void createServer();
    void clearConnections(QLocalSocket *socket = 0);
    void connectToServer(const QString &address);
    void connectSignals(QObject *obj);

    void writeSections(int id, const QByteArray &data, int socketId = 0);

private:
    Q_DECLARE_PUBLIC(QtRPCNetworkLocal)
    QtRPCNetworkLocal * const q_ptr;

private:
    const QString identifier_; //peer check

    const QString peerAddress_;

    QMap<QLocalSocket*, MsgSection*> sectionCtrls_;

    QSharedPointer<QLocalServer> localServer_;
    QSharedPointer<QLocalSocket> localSocketOut_;

    LanSearcher searcher_;

    int connectionType;

    int currentPaddings;

    bool exitMark;
};

#endif // LAN_QTRPCNETWORKTCPPRIVATE_H
