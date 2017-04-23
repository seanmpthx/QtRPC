#include "QtRPCMember.h"
#include <QDebug>

QtRPCMember::QtRPCMember()
{

}

void QtRPCMember::setKeywordItems(const QList<KeyWordItem> &itemType, const QList<KeyWordItem> &itemContent)
{
    itemType_ = itemType;
    itemContent_ = itemContent;
}

int QtRPCMember::parse()
{
    if (itemType_.isEmpty())
        RETURN_1;

    if (itemType_.count() > 1)
    {
        if (itemType_.at(1).type == KwLess &&
                !GlobalData::instance().isContains(itemType_.at(0).type))
        {
            RETURN_1;
        }
    }

    TypeItem typeMain;
    for (int i = 0; i < itemType_.count(); ++i)
    {
        const KeyWordItem &item = itemType_.at(i);
        bool contains = GlobalData::instance().isContains(item.type);
        if (contains)
        {
            typeMain.depends.append("<" + item.item + ">");
        }
        else
        {
            if (item.type == KwWord)
                typeMain.depends.append("\"" + item.item + "\"");
        }
        typeMain.text.append(item.item + " ");
        if (i == 0)
        {
            typeMain.type = item.item;
        }
    }

    bool pointer = itemType_.last().type == KwAsterisk;
    bool isOutArg = itemType_.first().type != KwConst;

    for (int i = 0; i < itemContent_.count(); )
    {
        const KeyWordItem *item = &(itemContent_.at(i++));
        bool pointerSecond = false;
        if (item->type == KwWord)
        {
            if (pointer)
            {
                pointerSecond = true;
            }
        }
        else if (item->type == KwAsterisk)
        {
            if (pointer)
                RETURN_1;
            pointerSecond = true;

            if (i >= itemContent_.count())
                RETURN_1;
            item = &(itemContent_.at(i++));
            if (item->type != KwWord)
                RETURN_1;
        }
        TypeItem typeCopy = typeMain;

        typeCopy.name = item->item;

        if (i == 0 && pointer)
        {
            typeCopy.isPointer = pointer;
            typeCopy.isOutArg = isOutArg;
        }
        else if (i > 0)
        {
            typeCopy.isPointer = pointerSecond;
            typeCopy.isOutArg = pointerSecond ? isOutArg : false;
        }

        if (i >= itemContent_.count())
            RETURN_1;
        item = &(itemContent_.at(i++));
        if (item->type != KwComma && item->type != KwSemicolon)
        {
            RETURN_1;
        }

        members_.append(typeCopy);
    }//end for


    for (int i = 0; i < members_.count(); ++i)
    {
        qDebug() << members_.at(i).text << " " << members_.at(i).name << " " << members_.at(i).depends;
    }

    return 0;
}

const QList<TypeItem> &QtRPCMember::getMembers()
{
    return members_;
}

