#ifndef QTRPCFUNCTION_H
#define QTRPCFUNCTION_H

#include <QList>
#include "types.h"

class QtRPCFunction
{
public:
    QtRPCFunction();
    void setKeywordItems(const QList<KeyWordItem> &itemRet, const QList<KeyWordItem> &itemContent);
    void setFuncType(FuncType type);
    void setAccessType(AccessType type);

    int parse();

    const FuncItem &getFunction();

private:
    int templateProc(int index, QStringList &depends);

private:
    FuncType funcType_;
    AccessType accessType_;

    FuncItem funcItem_;

    QList<KeyWordItem> itemRets_;
    QList<KeyWordItem> itemContent_;
};

#endif // QTRPCFUNCTION_H
