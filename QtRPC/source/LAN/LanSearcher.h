#ifndef LANKSEARCHER_H
#define LANKSEARCHER_H

#include <QObject>
#include <QSharedPointer>
#include <QTimer>
#include <QUuid>
#include <QUdpSocket>

enum SearchHeaderId
{
    SearchInvalid = 0,
    SearchRequest,
    SearchResponse
};

struct SearchData
{
    int msgId;
    QString applicationId; //self application check
    QString identifier; //self peer check.
    QString myIp;
    quint16 myPort;
    QUuid uuid;  // self object check
    int connectionType;
    SearchData() : msgId(0), myPort(0), connectionType(0){}
};

class LanSearcher : public QObject
{
    Q_OBJECT
public:
    LanSearcher(const QString &identifier, int ctype);

    void startSearch(const QString &myIp, quint16 port);
    void stopSearch();
    void closeSearch();

signals:
    void doGotPeer(const QString &peerAddress, const QString &peerIp, quint16 port);


private slots:
    void stopSearchSlot();
    void searchReadReady();
    void timerSearchMsg();

private:
    QString singleApplicationId();

private:
    const QString identifier_;
    int connectionType_;
    int maxPeers_;

    QUuid uuid_;

    QString myIp_;
    quint16 myPort_;

    QString currentPeerIp_;

    QString applicateionId;

    quint16 localSearchPort_;
    QSharedPointer<QUdpSocket> udpSocketSearch_;
    QTimer timerSearch_;
};

#endif // LANKSEARCHER_H
