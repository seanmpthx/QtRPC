#ifndef QTRPCCLASS_H
#define QTRPCCLASS_H

#include "QtRPCMember.h"
#include "QtRPCFunction.h"
#include <QList>
#include <QHash>
#include "types.h"

class QtRPCClass
{
public:
    QtRPCClass();
    void setKeywordItems(const QList<KeyWordItem> &items);
    void setClassInfo(const QString &className, const QString &baseClass, bool isClass = true);
    const QString className() const;
    const QString baseClass() const;
    bool isClassOrStruct() const;

    int parse();

    void getFuncItems(QList<FuncItem> *items);
    void getMemberItems(QList<TypeItem> *items);

private:
    int jumpFunction(int index);
    int jumpMemberListAndContent(int index);
    int beginWithSignLeft(int index, KeyWords left, KeyWords right);
    int functionCheck(int index, FuncType ftype, AccessType atype);
    int beginWithLess(int index);

    int pushFunction(int retBegin, int retEnd, int argBegin,
                     FuncType ftype, AccessType atype);
    int pushMember(int typeBegin, int typeEnd, int namesBegin, AccessType atype);

    int parseFunctionAndMember();

private:
    QList<KeyWordItem> kwis_;
    QString className_;
    QString baseClass_;
    bool isClassOrStruct_;

    QMultiHash<AccessType, QtRPCFunction> functions_;
    QMultiHash<AccessType, QtRPCMember> members_;
};

#endif // QTRPCCLASS_H
