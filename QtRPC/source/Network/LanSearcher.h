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

class LanSearcher : public QObject
{
    Q_OBJECT
public:
    LanSearcher();

    void startSearch(const QString &myIp, quint16 port);
    void stopSearch();

signals:
    void doGotPeer(const QString &peerAddress, const QString &peerIp, quint16 port);


private slots:
    void stopSearchSlot();
    void searchReadReady();
    void timerSearchMsg();

private:
    QString singleApplicationId();

private:
    QUuid uuid_;

    QString myIp_;
    quint16 myPort_;

    QString currentPeerIp_;

    quint16 localSearchPort_;
    QSharedPointer<QUdpSocket> udpSocketSearch_;
    QTimer timerSearch_;
};

#endif // LANKSEARCHER_H
