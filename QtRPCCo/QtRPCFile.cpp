#include "QtRPCFile.h"
#include <QTextStream>
#include <QDebug>
#include <QFile>
#include "types.h"

QtRPCFile::QtRPCFile(const QString &fileName) : fileName_(fileName)
{
    this->setFile(fileName);
}

void QtRPCFile::setFile(const QString &fileName)
{
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::Text | QIODevice::ReadOnly))
        return;

    fileName_ = fileName;

    QTextStream stream(&file);
    this->fileContent_ = stream.readAll();
    file.close();
}

bool QtRPCFile::isValid() const
{
    return !this->fileContent_.isEmpty();
}

const QString &QtRPCFile::fileName() const
{
    return fileName_;
}

int QtRPCFile::parse()
{
    prepareKeyword();

    int index = 0;
    while(fileContent_.count() > index)
    {
        QChar ch = fileContent_.at(index++);
        switch(ch.unicode())
        {
        case ' ':
            break;
        case '\r':
            break;
        case '\n':
            break;
        case '\t':
            break;
        case '#':
        {
            const QString inc("include");
            if (fileContent_.mid(index, inc.length()) == inc)
            {
                this->kwis_.append(KeyWordItem(KwInclude));
                index += inc.size();

                int ret = procHeader(fileContent_.midRef(index));
                if (ret < 0) RETURN_1;
                index += ret;
            }
            break;
        }
        case '/':
        {
            if (fileContent_.at(index) == '/')
            {
                index += 1;
                int ret = this->toNextChar1(fileContent_.midRef(index), '\n');
                if (ret < 0) return -2;
                //this->kwis.append(KeyWordItem(KwComment, fileContent_.mid(index, ret)));
            }
            break;
        }
        case '\"':
            break;
        case '.':
            break;
        case '{':
        {
            this->kwis_.append(KeyWordItem(KwBraceLeft, "{"));
            break;
        }
        case '}':
        {
            this->kwis_.append(KeyWordItem(KwBraceRight, "}"));
            if (fileContent_.at(index) == ';')
            {
                ++index;
            }
            break;
        }
        case '*':
        {
            this->kwis_.append(KeyWordItem(KwAsterisk, "*"));
            break;
        }
        case ':':
        {
            KeyWordItem &kwiCur = *(this->kwis_.insert(this->kwis_.end(), KeyWordItem(KwColon, ":")));
            if (fileContent_.at(index) == ':')
            {
                kwiCur.type = KwDomain;
                kwiCur.item = "::";
                ++index;
            }
            break;
        }
        case '<':
        {
            KeyWordItem &kwiCur = *(this->kwis_.insert(this->kwis_.end(), KeyWordItem(KwLess, "<")));
            if (fileContent_.at(index) == '<')
            {
                kwiCur.item = "<<";
                kwiCur.type = KwLeftMove;
                ++index;
            }
            break;
        }
        case '>':
        {
            KeyWordItem &kwiCur = *(this->kwis_.insert(this->kwis_.end(), KeyWordItem(KwGreater, ">")));
            if (fileContent_.at(index) == '>')
            {
                kwiCur.item = ">>";
                kwiCur.type = KwRightMove;
                ++index;
            }
            break;
        }
        case '&':
        {
            this->kwis_.append(KeyWordItem(KwAmpersand, "&"));
            break;
        }
        case ',':
        {
            this->kwis_.append(KeyWordItem(KwComma, ","));
            break;
        }
        case '(':
        {
            this->kwis_.append(KeyWordItem(KwParenLeft, "("));
            break;
        }
        case ')':
        {
            this->kwis_.append(KeyWordItem(KwParenRight, ")"));
            break;
        }
        case ';':
        {
            this->kwis_.append(KeyWordItem(KwSemicolon, ";"));
            break;
        }
        case '-':
        {
            if (fileContent_.at(index) == '>')
            {
                this->kwis_.append(KeyWordItem(KwPointer, "->"));
                ++index;
            }
            break;
        }
        case '~':
        {
            this->kwis_.append(KeyWordItem(KwTilde, "~"));
            break;
        }
        default:
        {
            QString word;
            if (ch >= '0' && ch <= '9')
            {
                int ret = checkNumber(fileContent_.midRef(index), ch, word);
                index += ret;
            }
            else
            {
                if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_')
                {
                    while ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_' || (ch >= '0' && ch <= '9'))
                    {
                        word.append(ch);
                        ch = fileContent_.at(index++);
                    }
                    KeyWordItem &kwiCur = *(this->kwis_.insert(this->kwis_.end(), KeyWordItem(KwWord, word)));
                    KeyWords kw = this->isKeyword(kwiCur.item);
                    --index;

                    if (kw == KwNone) break;
                    kwiCur.type = kw;
                }
            }

            break;
        }
        } //end witch
    }

    dumpKwis();

    ExtractClass();

    return 0;
}

void QtRPCFile::getFuncMembersItems(QHash<QString, ClassInfo> *classInfo)
{
    if (classInfo == 0)
        return;

    for (QList<QtRPCClass>::iterator itrCls = classes_.begin();
         itrCls != classes_.end(); ++itrCls)
    {
        QtRPCClass &cls = *itrCls;

        QHash<QString, ClassInfo>::iterator itrInfo = classInfo->find(cls.className());
        if (itrInfo == classInfo->end())
        {
            itrInfo = classInfo->insert(cls.className(), ClassInfo());
            itrInfo.value().headerFile = this->fileName();
            itrInfo.value().className = cls.className();
            itrInfo.value().baseClass = cls.baseClass();
            itrInfo.value().isClassOrStruct = cls.isClassOrStruct();
        }

        ClassInfo &info = itrInfo.value();

        cls.getFuncItems(&info.functions);
        cls.getMemberItems(&info.members);
    }
}


int QtRPCFile::procHeader(const QStringRef &ref)
{
    if(ref.isEmpty())
        return 0;

    int index = toNextChar2(ref, '<', '\"');
    if(index < 0) RETURN_1;
    QChar ch = *(ref.data() + index);

    index += 1;

    if (ch == '<' || ch == '\"')
    {
        if (ch == '<') ch = '>';

        int offset = toNextChar1(ref, ch, index);
        if (offset < 0) RETURN_1;

        KeyWordItem item(KwValueString, QString(ref.data() + index, offset - index));
        this->kwis_.append(item);

        return offset + 1;
    }

    RETURN_1;
}

void QtRPCFile::dumpKwis()
{
    const char *keytostr[] = {
        "KwNone", "KwComment", "KwWord",
        "KwValueFloat", "KwValueChar", "KwValueInt", "KwValueString",

        "KwInclude", "KwClass", "KwStruct", "KwPublic", "KwPrivate", "KwProtected", "KwSignal", "KwSlot", "KwVirtual",

        "KwPound", "KwColon", "KwParenLeft", "KwParenRight", "KwSemicolon",
        "KwAsterisk", "KwComma", "KwPointer", "KwTilde",

        "KwLess", "KwGreater", "KwLeftMove", "KwRightMove", "KwDomain", "KwAmpersand",
        "KwBracketLeft", "KwBracketRight", "KwBraceLeft", "KwBraceRight",

        "KwConst", "KwThis", "KwExplicit",
        "KwTypeBool", "KwTypeInt", "KwTypeVoid", "KwTypeFloat", "KwTypeDouble", "KwTypeChar", "KwTypeLong",
        "KwTypeInt8", "KwTypeUint8", "KwTypeInt16", "KwTypeUint16", "KwTypeInt32", "KwTypeUint32", "KwTypeInt64", "KwTypeUint64",
        "KwTypeQreal", "KwTypeQString", "KwTypeQByteArray", "KwTypeQList", "KwTypeQVector", "KwTypeQMap", "KwTypeQHash",
        "KwTypeQSet", "KwTypeQImage", "KwTypeQPixmap", "KwTypeQPair", "KwTypeQStringList"};

    int size = sizeof(keytostr);

    for (int i = 0; i < this->kwis_.count(); ++i)
    {
        const KeyWordItem &kw = this->kwis_.at(i);
        QString type;
        QString item = kw.item;

        if (kw.type >= 0 && kw.type < size)
        {
            type = keytostr[kw.type];
        }

        if (item.isEmpty())
            qDebug() << type;
        else
            qDebug() << type << "(" << item << ")";
    }
}

void QtRPCFile::prepareKeyword()
{
    keyType_.clear();
    keyType_.insert("const",     KwConst);
    keyType_.insert("this",      KwThis);
    keyType_.insert("explicit",  KwExplicit);

    keyType_.insert("class",     KwClass);
    keyType_.insert("struct",    KwStruct);
    keyType_.insert("private",   KwPrivate);
    keyType_.insert("public",    KwPublic);
    keyType_.insert("protected", KwProtected);
    keyType_.insert("signals",   KwSignal);
    keyType_.insert("slots",     KwSlot);
    keyType_.insert("virtual",   KwVirtual);


#define AddDefulatItem(name, type)  keyType_.insert(name, type); if (!GlobalData::instance().isContains(type)) GlobalData::instance().pushKeyWord(type)

    AddDefulatItem("bool", KwTypeBool);
    AddDefulatItem("int", KwTypeInt);
    AddDefulatItem("void", KwTypeVoid);
    AddDefulatItem("float", KwTypeFloat);
    AddDefulatItem("double", KwTypeDouble);
    AddDefulatItem("char", KwTypeChar);
    AddDefulatItem("long", KwTypeLong);

    AddDefulatItem("qint8", KwTypeInt8);
    AddDefulatItem("quint8", KwTypeUint8);
    AddDefulatItem("qint16", KwTypeInt16);
    AddDefulatItem("quint16", KwTypeUint16);
    AddDefulatItem("qint32", KwTypeInt32);
    AddDefulatItem("quint32", KwTypeUint32);
    AddDefulatItem("qint64", KwTypeInt64);
    AddDefulatItem("quint64", KwTypeUint64);

    AddDefulatItem("qreal", KwTypeQreal);
    AddDefulatItem("QString", KwTypeQString);
    AddDefulatItem("QByteArray", KwTypeQByteArray);
    AddDefulatItem("QList", KwTypeQList);
    AddDefulatItem("QVector", KwTypeQVector);
    AddDefulatItem("QMap", KwTypeQMap);
    AddDefulatItem("QHash", KwTypeQHash);

    AddDefulatItem("QSet", KwTypeQSet);
    AddDefulatItem("QImage", KwTypeQImage);
    AddDefulatItem("QPixmap", KwTypeQPixmap);
    AddDefulatItem("QPair", KwTypeQPair);
    AddDefulatItem("QStringList", KwTypeQStringList);


    //qDebug() << s_DefaultTypes;
}

int QtRPCFile::toNextChar1(const QStringRef &ref, QChar ch, int offset)
{
    if(ref.isEmpty()) return 0;

    QChar chNow = 0;
    for (int index = offset; index < ref.count(); ++index)
    {
        chNow = ref.at(index);
        if (chNow == ch)
            return index;
    }

    RETURN_1;
}

int QtRPCFile::toNextChar2(const QStringRef &ref, QChar ch1, QChar ch2, int offset)
{
    if(ref.isEmpty()) return 0;

    QChar chNow = 0;
    for (int index = offset; index < ref.count(); ++index)
    {
        chNow = ref.at(index);
        if (chNow == ch1 || chNow == ch2)
            return index;
    }

    RETURN_1;
}

int QtRPCFile::checkNumber(const QStringRef &ref, QChar ch, QString &word)
{
    int index = 0;

    word.append(ch);
    ch = ref.at(index++);
    if (ch == 'x' || ch == 'X')
    {
        word.append(ch);
        ch = ref.at(index++);
    }
    else if (ch == '.')
    {
        word.append(ch);
        ch = ref.at(index++);
    }

    while (ch >= '0' && ch <= '9' || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F'))
    {
        word.append(ch);
        ch = ref.at(index++);
    }

    if (word.contains('.'))
    {
        this->kwis_.append(KeyWordItem(KwValueFloat, word));
    }
    else
    {
        this->kwis_.append(KeyWordItem(KwValueInt, word));
    }
    --index;

    return index;
}

KeyWords QtRPCFile::isKeyword(const QString &word)
{
    QHash<QString, KeyWords>::iterator itr =
            keyType_.find(word);
    if (itr == keyType_.end())
        return KwNone;
    return itr.value();
}

void QtRPCFile::ExtractClass()
{
    for (int index = 0; index < this->kwis_.count(); )
    {
        const KeyWordItem *item = &(this->kwis_.at(index));
        if (item->type == KwInclude && (index + 1) < this->kwis_.count())
        {
            item = &(this->kwis_.at(index + 1));
            this->headerIncludes.append(item->item);

            index += 2;
            continue;
        }

        if (item->type == KwClass) //class
        {
            int indexNew = ExtractClass(index, true);
            index = indexNew;
            continue;
        }
        if (item->type == KwStruct) // struct
        {
            int indexNew = ExtractClass(index, false);
            index = indexNew;
            continue;
        }
        ++index;
    }

    qDebug() << "refrence header:" << this->headerIncludes;

    startClassContentParse();
}

int QtRPCFile::ExtractClass(int index, int isClassOrStruct)
{
    //int classBegin = index;
    ++index;//jump class keywork;

    const KeyWordItem *item = &(this->kwis_.at(index++)); //class name
    QString className = item->item;

    if (index >= this->kwis_.count())
        RETURN_1;

    item = &(this->kwis_.at(index++)); // : or {
    if (item->type == KwBraceLeft)
    {
        qDebug() << "class define:" << className;
        int classContentBegin = index;
        index = beginWithBraceLeft(index++);

        createClassParser(classContentBegin, index, isClassOrStruct, className);
        return index;
    }
    if (item->type != KwColon)
        RETURN_1;
    if (index >= this->kwis_.count())
        RETURN_1;

    item = &(this->kwis_.at(index++)); // private public protected
    if (item->type != KwPrivate && item->type != KwPublic && item->type != KwProtected)
    {
        if (item->type != KwWord)
            RETURN_1; //default private
        --index;
    }
    if (index >= this->kwis_.count())
        RETURN_1;

    item = &(this->kwis_.at(index++)); //base class
    if (item->type != KwWord)
        RETURN_1; //class define line end.

    QString baseClass = item->item;

    if (index >= this->kwis_.count())
        RETURN_1;

    item = &(this->kwis_.at(index++)); // <
    if (item->type == KwBraceLeft)
    {
        qDebug() << "class define:" << className << " base class:" << baseClass;
        int classContentBegin = index;
        index = beginWithBraceLeft(index++);

        createClassParser(classContentBegin, index, isClassOrStruct, className, baseClass);
        return index;
    }

    if (item->type != KwLess)
        RETURN_1;
    if (index >= this->kwis_.count())
        RETURN_1;

    item = &(this->kwis_.at(index++)); // template class type
    if (item->type != KwWord)
        RETURN_1;

    QString templateType = item->item;

    item = &(this->kwis_.at(index++)); // >
    if (item->type != KwGreater)
        RETURN_1;

    //done class define line.
    qDebug() << "class define:" << className
             << " base class:" << QString("%1<%2>").arg(baseClass, templateType);

    if (index >= this->kwis_.count())
        RETURN_1;
    item = &(this->kwis_.at(index++));

    if (item->type == KwBraceLeft)
    {
        int classContentBegin = index;
        index = beginWithBraceLeft(index++);

        createClassParser(classContentBegin, index, isClassOrStruct, className,
                          QString("%1<%2>").arg(baseClass, templateType));
        return index;
    }
    RETURN_1;
}

int QtRPCFile::beginWithBraceLeft(int index)
{
    if (index >= this->kwis_.count())
    {
        RETURN_1;
    }

    int indexStart = index;

    for (; index < this->kwis_.count(); ++index)
    {
        const KeyWordItem *item = &(this->kwis_.at(index));
        if (item->type == KwBraceLeft)
        {
            index = this->beginWithBraceLeft(index + 1);
        }
        if (item->type == KwBraceRight)
        {
            if (indexStart == index)
                return index;
            return index;
        }
    } //end for

    RETURN_1;
}

void QtRPCFile::createClassParser(int posStart, int posEnd,
                                  bool isClassOrStruct,
                                  const QString &className,
                                  const QString &baseClass)
{
    QList<KeyWordItem> clsItems = this->kwis_.mid(posStart, posEnd - posStart - 1);
    if (clsItems.isEmpty())
        return;

    QtRPCClass &cls = *(classes_.insert(classes_.end(), QtRPCClass()));
    cls.setKeywordItems(clsItems);
    cls.setClassInfo(className, baseClass, isClassOrStruct);
}

void QtRPCFile::startClassContentParse()
{
    for (QList<QtRPCClass>::iterator itr = classes_.begin(); itr != classes_.end(); ++itr)
    {
        QtRPCClass &cls = *itr;
        if (cls.parse() < 0)
        {
            qDebug() << "parse failed, " << cls.className();
            return;
        }
    }
}


