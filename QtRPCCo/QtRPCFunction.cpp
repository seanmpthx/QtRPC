#include "QtRPCFunction.h"
#include <QDebug>

QtRPCFunction::QtRPCFunction()
{

}

void QtRPCFunction::setKeywordItems(const QList<KeyWordItem> &itemRet, const QList<KeyWordItem> &itemContent)
{
    itemRets_ = itemRet;
    itemContent_ = itemContent;
}

void QtRPCFunction::setFuncType(FuncType type)
{
    funcType_ = type;
}

void QtRPCFunction::setAccessType(AccessType type)
{
    accessType_ = type;
}

int QtRPCFunction::parse()
{
    if (itemRets_.isEmpty())
        RETURN_1;

    for (int i = 0; i < itemRets_.count(); ++i)
    {
        const KeyWordItem &item = itemRets_.at(i);
        bool contains = GlobalData::instance().isContains(item.type);
        if (contains)
        {
            if (item.type != KwTypeVoid)
                funcItem_.retType.depends.append("<" + item.item + ">");
        }
        else
        {
            if (item.type == KwWord)
                funcItem_.retType.depends.append("\"" + item.item + "\"");
        }
        funcItem_.retType.text.append(item.item + " ");
    }
    if (itemRets_.last().type == KwAsterisk)
    {
        funcItem_.retType.isPointer = true;
    }

    if (itemRets_.count() > 2)
    {
        funcItem_.retType.type = itemRets_.first().item;
    }
    else
    {
        funcItem_.retType.type = itemRets_.first().item;
    }

    if (itemContent_.isEmpty())
        RETURN_1;

    funcItem_.name = itemContent_.first().item;

    qDebug() << funcItem_.retType.text << " " << funcItem_.name;

    for (int i = 1; i < itemContent_.count(); )
    {
        const KeyWordItem *item = &(itemContent_.at(i++));
        if (item->type != KwParenLeft && i == 1)
            RETURN_1;

        if (i >= itemContent_.count())
            RETURN_1;
        item = &(itemContent_.at(i++));

        if (item->type == KwParenRight)
        {
            if (i >= itemContent_.count())
                RETURN_1;
            item = &(itemContent_.at(i++));
            if (item->type != KwSemicolon)
                RETURN_1;
            continue;
        }

        int argBegin = i - 1;

        TypeItem typeItem;
        bool isOutArgNull = false;
        if (item->type == KwConst)
        {
            if (i >= itemContent_.count())
                RETURN_1;
            item = &(itemContent_.at(i++));
            typeItem.isOutArg = false;
            isOutArgNull = true;
        }

        if (item->type == KwWord && !GlobalData::instance().isContains(item->type))
        {
            typeItem.depends.append("\"" + item->item + "\"");
            typeItem.type = item->item;
            //no template
            if (i >= itemContent_.count())
                RETURN_1;
            item = &(itemContent_.at(i++));
            if (item->type == KwDomain)
            {
                qDebug() << "not supported syntax like this : " << typeItem.type + item->item;
                RETURN_1;
            }
        }
        else if (item->type == KwWord)
        {
            typeItem.type = item->item;

            if (i >= itemContent_.count())
                RETURN_1;
            item = &(itemContent_.at(i++));
            if (item->type == KwLess) //template
            {
                i = templateProc(i, typeItem.depends);
                if (i < 0)
                    RETURN_1;
            }
        }
        else if (GlobalData::instance().isContains(item->type))
        {
            typeItem.type = item->item;
            if (i >= itemContent_.count())
                RETURN_1;
            item = &(itemContent_.at(i++));
            if (item->type == KwLess) //template
            {
                i = templateProc(i, typeItem.depends);
                if (i < 0)
                    RETURN_1;
            }
        }
        else
        {
            RETURN_1;
        }

        if (item->type == KwAmpersand)
        {
            if (i >= itemContent_.count())
                RETURN_1;
            item = &(itemContent_.at(i++));

            if (!isOutArgNull)
                typeItem.isOutArg = true;
        }
        else if (item->type == KwAsterisk)
        {
            if (i >= itemContent_.count())
                RETURN_1;
            typeItem.isPointer = true;
            item = &(itemContent_.at(i++));

            if (!isOutArgNull)
                typeItem.isOutArg = true;
        }

        int argEnd = i - 1;
        //push text;
        for (int t = argBegin; t < argEnd; ++t)
        {
            typeItem.text.append(itemContent_.at(t).item + " ");
        }

        if (item->type != KwWord)
            RETURN_1;

        typeItem.name = item->item;

        funcItem_.args.append(typeItem);

        if (i >= itemContent_.count())
            RETURN_1;
        item = &(itemContent_.at(i++));
        if (item->type == KwComma)
        {
            i -= 1;
            continue;
        }
        else if (item->type == KwParenRight)
        {
            if (i >= itemContent_.count())
                RETURN_1;
            item = &(itemContent_.at(i++));
            if (item->type != KwSemicolon)
                RETURN_1;
        }

    } //end for

    if (!funcItem_.args.isEmpty())
    {
        for (int i = 0; i < funcItem_.args.count(); ++i)
            qDebug() << "args:" << funcItem_.args.at(i).text << " " << funcItem_.args.at(i).name;
    }
    qDebug() << funcItem_.retType.depends;

    return 0;
}

const FuncItem &QtRPCFunction::getFunction()
{
    return funcItem_;
}

int QtRPCFunction::templateProc(int index, QStringList &depends)
{
    const KeyWordItem *item = 0;

    do
    {
        if (index < 0)
            RETURN_1;

        if (index >= this->itemContent_.count())
            RETURN_1;
        item = &(this->itemContent_.at(index++));

        if (item->type != KwComma && item->type != KwLess && item->type != KwGreater)
        {
            if (item->type != KwWord)
            {
                if (GlobalData::instance().isContains(item->type))
                    depends.append("<" + item->item + ">");
                else
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
                index = templateProc(index, depends);
            }
        }

    } while(item->type != KwGreater);

    if (item->type == KwGreater)
        return index;

    RETURN_1;
}

