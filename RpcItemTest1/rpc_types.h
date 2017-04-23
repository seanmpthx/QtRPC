#ifndef RPC_TYPES_H
#define RPC_TYPES_H

#include <QString>
#include <QStringList>

struct DataTest1
{
    QString arg1;
    int arg2;
    char arg3;
};

struct DataTest2
{
    QStringList arg1;
};

#endif //RPC_TYPES_H
