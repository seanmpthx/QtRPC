#ifndef QTRPCMEMBER_H
#define QTRPCMEMBER_H

#include <QList>
#include "types.h"

class QtRPCMember
{
public:
    QtRPCMember();
    void setKeywordItems(const QList<KeyWordItem> &itemType, const QList<KeyWordItem> &itemContent);

    int parse();

    const QList<TypeItem> &getMembers();

private:
    QList<KeyWordItem> itemType_;
    QList<KeyWordItem> itemContent_;
    QList<TypeItem> members_;
};

#endif // QTRPCMEMBER_H
