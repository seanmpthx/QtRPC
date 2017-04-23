#ifndef MSGSECTION_H
#define MSGSECTION_H

#include <QHash>
#include <QIODevice>

class MsgSection : public QObject
{
    Q_OBJECT
public:
    MsgSection(int descriptor, QIODevice *device);
    int descriptor() const;

    void read();
    void write(int msgId, const QByteArray &data, int socketId);

signals:
    void doGotMsg(int unique, const QByteArray &data, int socketId);

private:
    const int descriptor_;
    QIODevice * const device;
    quint32 packageSize;
    //subid, data;
    QHash<int, QByteArray> packages;
};

#endif // MSGSECTION_H
