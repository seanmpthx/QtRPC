#include "QtRPCClass.h"
#include <QDebug>

QtRPCClass::QtRPCClass() : isClassOrStruct_(true)
{
}

void QtRPCClass::setKeywordItems(const QList<KeyWordItem> &items)
{
    kwis_ = items;
}

void QtRPCClass::setClassInfo(const QString &className, const QString &baseClass, bool isClass)
{
    className_ = className;
    baseClass_ = baseClass;
    isClassOrStruct_ = isClass;
}

const QString QtRPCClass::className() const
{
    return className_;
}

const QString QtRPCClass::baseClass() const
{
    return baseClass_;
}

bool QtRPCClass::isClassOrStruct() const
{
    return isClassOrStruct_;
}

int QtRPCClass::parse()
{
    AccessType accessType = isClassOrStruct_ ? AccessPublic : AccessPrivate;
    FuncType funcType = FuncFunction;
    for (int index = 0; index < this->kwis_.count(); )
    {
        const KeyWordItem *item = &(this->kwis_.at(index));

        if (item->type == KwBraceRight && index + 1 >= this->kwis_.count())
        {
            return 0;
        }

        if (item->type == KwPublic || item->type == KwPrivate || item->type == KwProtected)
        {
            index += 1;
            if (index >= this->kwis_.count())
                RETURN_1;

            item = &(this->kwis_.at(index++));
            if (item->type == KwColon)
            {
                continue;
            }
            else if (item->type == KwSlot)
            {
                funcType = FuncSlot;
                item = &(this->kwis_.at(index++));
                if (item->type != KwColon)
                    RETURN_1;
                continue;
            }
            else if (item->type == KwSignal)
            {
                funcType = FuncSignal;
                item = &(this->kwis_.at(index++));
                if (item->type != KwColon)
                    RETURN_1;
                continue;
            }

            RETURN_1;
        }

        if (item->type == KwExplicit)
        {
            index += 1;
            if (index >= this->kwis_.count())
                RETURN_1;
            continue;
        }
        else if (item->type == KwWord && item->item == className_) //constructor
        {
            if (index >= this->kwis_.count())
                RETURN_1;
            item = &(this->kwis_.at(index++));

            //jump structure function.
            index = jumpFunction(index - 1);
            if (index < 0)
                RETURN_1;
            ++index;
            if (index >= this->kwis_.count())
                RETURN_1;
            item = &(this->kwis_.at(index++));
            if (item->type == KwSemicolon)
                return index;
            if (item->type == KwColon)
            {
                //member init list
                index = jumpMemberListAndContent(index);
                if(index < 0)
                    RETURN_1;
                continue;
            }
            RETURN_1;
        }
        else if (item->type == KwTilde) //destructor
        {
            if (index >= this->kwis_.count())
                RETURN_1;
            item = &(this->kwis_.at(index++));

            if (item->item == className_)
            {
                //jump destructure function.
                index = jumpFunction(index - 1);
                if (index < 0)
                    RETURN_1;
                continue;
            }
        }
        else if (item->type == KwVirtual)
        {
            if (index >= this->kwis_.count())
                RETURN_1;
            item = &(this->kwis_.at(index++));
            if (item->type == KwWord || GlobalData::instance().isContains(item->type))
            {
                index = functionCheck(index - 1, funcType, accessType);
                if (index < 0)
                    RETURN_1;
                continue;
            }
            else if (item->type == KwTilde) //destructure
            {
                index = jumpFunction(index - 1);
                if (index < 0)
                    RETURN_1;
                continue;
            }
        }
        else if (item->type == KwConst)
        {
            index = functionCheck(index, funcType, accessType);
            if (index < 0)
                RETURN_1;
            continue;
        }
        else if (item->type == KwWord || GlobalData::instance().isContains(item->type))
        {
            index = functionCheck(index, funcType, accessType);
            if (index < 0)
                RETURN_1;
            continue;
        }

        //qDebug() << s_DefaultTypes;
        RETURN_1;
    } //end for


    return parseFunctionAndMember();
}

void QtRPCClass::getFuncItems(QList<FuncItem> *items)
{
    if (items == 0)
        return;
    for (QMultiHash<AccessType, QtRPCFunction>::iterator itr = functions_.begin();
         itr != functions_.end(); ++itr)
    {
        const FuncItem &item = itr.value().getFunction();
        items->append(item);
    }
}

void QtRPCClass::getMemberItems(QList<TypeItem> *items)
{
    if (items == 0)
        return;
    for (QMultiHash<AccessType, QtRPCMember>::iterator itr = members_.begin();
         itr != members_.end(); ++itr)
    {
        *items << itr.value().getMembers();
    }
}

int QtRPCClass::jumpFunction(int index)
{
    if (index >= this->kwis_.count())
        RETURN_1;
    const KeyWordItem *item = &(this->kwis_.at(index++));

    if (item->type != KwWord)
        RETURN_1;

    if (index >= this->kwis_.count())
        RETURN_1;
    item = &(this->kwis_.at(index++));

    if (item->type == KwParenLeft)
    {
        index = beginWithSignLeft(index, KwParenLeft, KwParenRight);
        return index;
    }

    if (index >= this->kwis_.count())
        RETURN_1;
    item = &(this->kwis_.at(index++));
    if (item->type == KwComma)
    {
        return index;
    }
    else if (item->type == KwBracketLeft)
    {
        index = beginWithSignLeft(index, KwBracketLeft, KwBracketRight);
        return index;
    }

    return 0;
}

int QtRPCClass::jumpMemberListAndContent(int index)
{
    const KeyWordItem *item = 0;
    //i hate expressions.

    //to next braceright, if error, i've no idea, change it by yourself. lgpl
    do
    {
        if (index >= this->kwis_.count())
            RETURN_1;
        item = &(this->kwis_.at(index++));

    } while(item != 0 && item->type != KwBraceRight);

    if (item->type == KwBraceRight)
        return index;

    RETURN_1;
}

int QtRPCClass::beginWithSignLeft(int index, KeyWords left, KeyWords right)
{
    if (index >= this->kwis_.count())
        RETURN_1;

    int indexStart = index;

    for (; index < this->kwis_.count(); ++index)
    {
        const KeyWordItem *item = &(this->kwis_.at(index));
        if (item->type == left)
        {
            if (index + 1 < this->kwis_.count() && this->kwis_.at(index + 1).type == right)
            {
                ++index;
                continue;
            }

            index = this->beginWithSignLeft(index + 1, left, right);
        }
        if (item->type == right)
        {
            if (indexStart == index)
                return index + 1;
            return index;
        }
    } //end for

    return index;
}

int QtRPCClass::functionCheck(int index, FuncType ftype, AccessType atype)
{
    int tempTypeBegin = index;

    if (index >= this->kwis_.count())
        RETURN_1;
    const KeyWordItem *item = &(this->kwis_.at(index++));
    if (index >= this->kwis_.count())
        RETURN_1;
    if (item->type == KwConst)
    {
        if (index >= this->kwis_.count())
            RETURN_1;
        item = &(this->kwis_.at(index++));
    }

    bool isDefaultType = GlobalData::instance().isContains(item->type);

    item = &(this->kwis_.at(index++));


    if (item->type == KwLess)
    {
        if (index >= this->kwis_.count())
            RETURN_1;
        if (!isDefaultType)
            RETURN_1;

        index = beginWithLess(index);
        if (index < 0)
            RETURN_1;

        int tempTypeEnd = index;

        if (index >= this->kwis_.count())
            RETURN_1;
        item = &(this->kwis_.at(index++));
        if (item->type == KwAsterisk)
        {
            tempTypeEnd = index;
            item = &(this->kwis_.at(index++));
        }

        if (item->type == KwWord)
        {
            if (index >= this->kwis_.count())
                RETURN_1;
            item = &(this->kwis_.at(index++));
            if (item->type != KwParenLeft)
            {
                if (item->type == KwSemicolon || item->type == KwComma)
                {
                    return pushMember(tempTypeBegin, tempTypeEnd, index - 2, atype);
                }
                RETURN_1;
            }

            return pushFunction(tempTypeBegin, tempTypeEnd, index - 2, ftype, atype);
        }

        RETURN_1;
    }
    else
    {
        int tempTypeEnd = index - 1;
        if (item->type == KwAsterisk)
        {
            item = &(this->kwis_.at(index++));
            tempTypeEnd = index - 1;
        }

        if (index >= this->kwis_.count())
            RETURN_1;
        item = &(this->kwis_.at(index++));
        if (item->type != KwParenLeft)
        {
            if (item->type == KwSemicolon || item->type == KwComma)
            {
                return pushMember(tempTypeBegin, tempTypeEnd, index - 2, atype);
            }
            RETURN_1;
        }

        return pushFunction(tempTypeBegin, tempTypeEnd, index - 2, ftype, atype);
    }
    RETURN_1;
}

int QtRPCClass::beginWithLess(int index)
{
    const KeyWordItem *item = &(this->kwis_.at(index));
    do
    {

        if (index < 0)
            RETURN_1;
        if (index >= this->kwis_.count())
            RETURN_1;
        item = &(this->kwis_.at(index++));

        if (item->type != KwComma && item->type != KwLess && item->type != KwGreater)
        {
            if (item->type != KwWord && !GlobalData::instance().isContains(item->type))
            {
                RETURN_1;
            }
        }
        else
        {
            if (item->type == KwComma)
            {
                continue;
            }

            if (item->type == KwLess)
            {
                index = beginWithLess(index);
            }
        }

    } while(item->type != KwGreater);

    if (item->type == KwGreater)
        return index;
    RETURN_1;
}

int QtRPCClass::pushFunction(int retBegin, int retEnd, int argBegin, FuncType ftype, AccessType atype)
{
    QList<KeyWordItem> retItems = this->kwis_.mid(retBegin, retEnd - retBegin);

    const KeyWordItem *item = 0;

    int index = argBegin;

    do
    {
        if (index >= this->kwis_.count())
            RETURN_1;
        item = &(this->kwis_.at(index++));

    } while(item->type != KwSemicolon);

    if (item->type == KwSemicolon)
    {
        QList<KeyWordItem> content = this->kwis_.mid(argBegin, index - argBegin);
        QtRPCFunction &rpcFun = functions_.insert(atype, QtRPCFunction()).value();
        rpcFun.setKeywordItems(retItems, content);
        rpcFun.setFuncType(ftype);
        rpcFun.setAccessType(atype);
        return index;
    }

    RETURN_1;
}

int QtRPCClass::pushMember(int typeBegin, int typeEnd, int namesBegin, AccessType atype)
{
    QList<KeyWordItem> typeItems = this->kwis_.mid(typeBegin, typeEnd - typeBegin);

    const KeyWordItem *item = 0;

    int index = namesBegin;

    do
    {
        if (index >= this->kwis_.count())
            RETURN_1;
        item = &(this->kwis_.at(index++));

    } while(item->type != KwSemicolon);

    if (item->type == KwSemicolon)
    {
        QList<KeyWordItem> content = this->kwis_.mid(namesBegin, index - namesBegin);
        QtRPCMember &rpcMemb = members_.insert(atype, QtRPCMember()).value();
        rpcMemb.setKeywordItems(typeItems, content);
        return index;
    }

    RETURN_1;
}

int QtRPCClass::parseFunctionAndMember()
{
    for (QHash<AccessType, QtRPCFunction>::iterator itr = functions_.begin();
         itr != functions_.end(); ++itr)
    {
        if (itr.value().parse() < 0)
            RETURN_1;
    }

    for (QHash<AccessType, QtRPCMember>::iterator itr = members_.begin();
         itr != members_.end(); ++itr)
    {
        if (itr.value().parse() < 0)
            RETURN_1;
    }
    return 0;
}
