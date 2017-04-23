#ifndef TYPES
#define TYPES

#include <QString>
#include <QStringList>

#define RETURN_1 do{ qDebug() << __FUNCTION__ << " " << __LINE__; return -1;  }while(false)

enum KeyWords
{
    KwNone, KwComment, KwWord,
    KwValueFloat, KwValueChar, KwValueInt, KwValueString,

    KwInclude, KwClass, KwStruct, KwPublic, KwPrivate, KwProtected, KwSignal, KwSlot, KwVirtual,

    KwPound /*#*/, KwColon/*:*/,  KwParenLeft/*(*/, KwParenRight/*)*/, KwSemicolon /*;*/,
    KwAsterisk/***/, KwComma/*,*/, KwPointer/*->*/, KwTilde/*~*/,

    KwLess/*<*/, KwGreater/*>*/, KwLeftMove/*<<*/, KwRightMove/*>>*/, KwDomain/*::*/,  KwAmpersand/*&*/,
    KwBracketLeft/*[*/, KwBracketRight/*]*/, KwBraceLeft/*{*/, KwBraceRight/*}*/,

    KwConst, KwThis, KwExplicit,
    KwTypeBool, KwTypeInt, KwTypeVoid, KwTypeFloat, KwTypeDouble, KwTypeChar, KwTypeLong,
    KwTypeInt8, KwTypeUint8, KwTypeInt16, KwTypeUint16, KwTypeInt32, KwTypeUint32, KwTypeInt64, KwTypeUint64,
    KwTypeQreal, KwTypeQString, KwTypeQByteArray, KwTypeQList, KwTypeQVector, KwTypeQMap, KwTypeQHash,
    KwTypeQSet, KwTypeQImage, KwTypeQPixmap, KwTypeQPair, KwTypeQStringList


};


struct KeyWordItem
{
    QString item;
    KeyWords type;
    KeyWordItem(KeyWords type, const QString &item = QString()) :item(item), type(type)
    {
    }

};

enum AccessType
{
    AccessPrivate,
    AccessPublic,
    AccessProtected,
};

enum FuncType
{
    FuncFunction,
    FuncSignal,
    FuncSlot
};

struct TypeItem
{
    QString type;
    QString text;
    QString name;
    bool isPointer;
    bool isOutArg;

    QStringList depends;
    TypeItem() : isPointer(false), isOutArg(false){}
};

struct FuncItem
{
    TypeItem retType;
    QString name;
    QList<TypeItem> args;
};

struct ClassInfo
{
    QString headerFile;

    //class or struct name.
    QString className;

    //single inherit just.
    QString baseClass;

    //true=class false=struct
    bool isClassOrStruct;

    QList<FuncItem> functions;
    QList<TypeItem> members;
};

class GlobalData
{
public:
    static GlobalData &instance()
    {
        static GlobalData sGobalData;
        return sGobalData;
    }

    void pushKeyWord(KeyWords key)
    {
        defaultTypes.append(key);
    }

    bool isContains(KeyWords key)
    {
        return defaultTypes.contains(key);
    }

private:
    QList<KeyWords> defaultTypes;
};

#endif // TYPES

