#ifndef MSGSECTION_H
#define MSGSECTION_H

#include <QHash>
#include <QIODevice>

class MsgSection : public QObject
{
    Q_OBJECT
public:
    MsgSection(QIODevice *device);

    void read();
    void write(int msgId, const QByteArray &data);

signals:
    void doGotMsg(int unique, const QByteArray &data);

private:
    QIODevice * const device;
    quint32 packageSize;
    //subid, data;
    QHash<int, QByteArray> packages;
};

#endif // MSGSECTION_H
