#include "QtRPCGenerator.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextCodec>

QtRPCGenerator::QtRPCGenerator() : funcIndex_(0)
{

}

void QtRPCGenerator::setHeaderPath(const QString &path)
{
    filePath_ = path;
}

void QtRPCGenerator::setHeaderFile(const QString &fileName)
{
    headerFileName_ = fileName;
}

void QtRPCGenerator::setHeaderInfo(const QHash<QString, ClassInfo> &info)
{
    if (info.isEmpty())
        return;
    for (QHash<QString, ClassInfo>::const_iterator itr = info.begin();
         itr != info.end(); ++itr)
    {
        const ClassInfo &cls = itr.value();
        if (cls.baseClass == "QtRPCItem" && headerFunctions_.isEmpty())
        {
            headerClassName_ = cls.className;
            headerFunctions_ = cls.functions;
            headerFileName_ = cls.headerFile;
            continue;
        }
        refrenceMembers_.insert(cls.className, cls);
    }
}

void QtRPCGenerator::setRefrenceInfo(const QHash<QString, ClassInfo> &info)
{
    for (QHash<QString, ClassInfo>::const_iterator itr = info.begin();
         itr != info.end(); ++itr)
    {
        const ClassInfo &cls = itr.value();
        refrenceMembers_.insert(cls.className, cls);
    }
}

bool QtRPCGenerator::startGenerate()
{
    for (int i = 0; i < headerFunctions_.count(); ++i)
    {
        qDebug() << headerFunctions_.at(i).name;
    }

    QString funcIds("\r\n");
    QStringList depends;
    QStringList headers;
    QPair<QString, QString> pairContent = createRpcFuncContent(&funcIds, &depends, &headers);
    QString &headerFunctionBody = pairContent.first;
    QString typeSerialFunction = createTypeFunction(depends, &headers);

    headers.append("#include <QDataStream>");

    saveToFunctions(headers, typeSerialFunction, funcIds, headerFunctionBody);

    //the first callback's header is not needed.
    headers.pop_front();
    saveToCallbacks(typeSerialFunction, pairContent.second, headers, funcIds);

    return true;
}

QPair<QString, QString> QtRPCGenerator::createRpcFuncContent(QString *functionId,
                                                             QStringList *depends,
                                                             QStringList *headers)
{
    funcIndex_ = headerFunctions_.count();

    QPair<QString, QString> pairRet;

    QString retFuncValue, retCallbackValue;

    for (QList<FuncItem>::const_iterator itrFunc = headerFunctions_.begin();
         itrFunc != headerFunctions_.end(); ++itrFunc)
    {
        const FuncItem &item = *itrFunc;
        pushDepends(item.retType.depends, depends);

        QString argsDef, argsSignature, outArgs, inArgs, inArgsVoid, invokeArg;
        for (QList<TypeItem>::const_iterator itrArg = item.args.begin(); itrArg != item.args.end(); ++itrArg)
        {
            const TypeItem &arg = *itrArg;

            //arg sig
            if (!argsSignature.isEmpty())
                argsSignature.append(", ");
            argsSignature.append(arg.text + arg.name);

            //arg def
            QString argTypePureTemp = arg.text.trimmed();
            if (argTypePureTemp.left(5) == "const")
            {
                argTypePureTemp = argTypePureTemp.mid(5);
            }
            if (argTypePureTemp.right(1) == "&" || argTypePureTemp.right(1) == "*")
            {
                argTypePureTemp = argTypePureTemp.mid(0, argTypePureTemp.length() - 1);
            }
            argTypePureTemp = argTypePureTemp.trimmed();
            argsDef.append(argTypePureTemp + " " + arg.name + ";");
            if (!invokeArg.isEmpty())
                invokeArg.append(",");

            invokeArg.append(QString("QArgument<%1 >(\"%2\", %3)").arg(argTypePureTemp, argTypePureTemp, arg.name));

            inArgsVoid.append(" << &" + arg.name);

            //arg in
            if (arg.isPointer)
            {
                inArgs.append(" << *" + arg.name);
            }
            else
            {
                inArgs.append(" << " + arg.name);
            }

            //arg out
            outArgs.append(" >> " + arg.name);

            //depends
            pushDepends(arg.depends, depends);

        } //end for (QList<TypeItem>::const_iterator

        QString signature = item.retType.text + headerClassName_ + "::" + item.name + QString("(%1)").arg(argsSignature);

        QString funcUnique = pushFunctionId(signature, item.name, functionId);

        QString callbackOut = createCallbackOutputContent(funcUnique, argsDef, outArgs,
                                                          inArgsVoid, item.retType.text, invokeArg);

        QString funcBody = createFunctionContent(signature, item.retType.text, inArgs, funcUnique);

        retFuncValue.append(funcBody);
        retCallbackValue.append(callbackOut);
    }
    pushHeaders(headerFileName_, headers);

    QString callbackFunc = "switch(unique)\r\n        {\r\n%1        }\r\n";
    pairRet.second = callbackFunc.arg(retCallbackValue);
    pairRet.first = retFuncValue;

    return pairRet;
}

QString QtRPCGenerator::createTypeFunction(const QStringList &types, QStringList *headers)
{
    const QString strIn =
            "static QDataStream &operator <<(QDataStream &stream, const %1 &%2)\r\n"
            "{\r\n"
            "    stream %3;\r\n"
            "    return stream;\r\n"
            "}\r\n";

    const QString strOut =
            "static QDataStream &operator >>(QDataStream &stream, %1 &%2)\r\n"
            "{\r\n"
            "    stream %3;\r\n"
            "    return stream;\r\n"
            "}\r\n";

    QString retLines;

    foreach(const QString &type, types)
    {
        QString typeFind = type.mid(1, type.size() - 2);
        QHash<QString, ClassInfo>::iterator itrCls = refrenceMembers_.find(typeFind);
        if (itrCls == refrenceMembers_.end())
        {
            if(typeFind.at(0) == 'Q')
            {
                headers->append("#include " + type);
            }
            continue;
        }
        const ClassInfo &clsInfo = itrCls.value();

        const QString &clsName = clsInfo.className;
        QString varName = clsName.toLower() + "_";

        QString memberIn, memberOut;
        for (int i = clsInfo.members.count() - 1; i >= 0; --i)
        {
            const TypeItem &member = clsInfo.members.at(i);
            QString varExp = varName + "." + member.name;
            QString pointerValue = QString("(" + varExp + " == 0 ? %1 : *%2)").arg(member.type + "()", varExp);
            QString pointer = member.isPointer ? pointerValue : varExp;
            if (member.type == "char")
            {
                //= =!
                memberIn.append(" << (qint8)" + pointer);
                memberOut.append(" >> *(qint8*)(&" + pointer + ")");
            }
            else
            {

                memberIn.append(" << " + pointer);
                memberOut.append(" >> " + pointer);
            }

            if (i > 0 && i % 3 == 0)
            {
                memberIn.append("\r\n   ");
                memberOut.append("\r\n   ");\
            }
        }

        QString lineIn = strIn.arg(clsName, varName, memberIn);
        QString lineOut = strOut.arg(clsName, varName, memberOut);
        retLines.append(lineIn);
        retLines.append(lineOut);
        retLines.append("\r\n");

        pushHeaders(clsInfo.headerFile, headers);
    }

    return retLines;
}

void QtRPCGenerator::pushDepends(const QStringList &dependsIn, QStringList *dependsOut)
{
    if (dependsOut == 0)
        return;
    for (int i = 0; i < dependsIn.count(); ++i)
    {
        const QString &depend = dependsIn.at(i);
        if (dependsOut->contains(depend))
            continue;
        dependsOut->append(depend);
    }
}

QString QtRPCGenerator::pushFunctionId(const QString &funcSign, const QString &name, QString *funcId)
{
    if (funcId == 0)
        return QString();

    QString funcIdName = "rpc_functoin_" + name.toLower();
    QString funcIdDef = QString("const int %1 = %2;//%3\r\n")
            .arg(funcIdName, QString::number(funcIndex_--), funcSign);

    funcId->append(funcIdDef);

    return funcIdName;
}

void QtRPCGenerator::pushHeaders(const QString &header, QStringList *headers)
{
    const QString includeLine("#include \"%1\"");
    QString headerLine = includeLine.arg(QFileInfo(header).fileName());
    if (headers->contains(headerLine))
        return;
    headers->append(headerLine);
}

QString QtRPCGenerator::createReturnContent(const QString &retArgText)
{
    bool isPointer = false;
    QString retDef = createReturnTypeDef(retArgText, &isPointer);
    if (retDef.isEmpty())
        return QString();

    QString retLine =
            "    %1\r\n"
            "    const QByteArray &rpc_ret_data = loop.data();\r\n"
            "    QDataStream rpc_stream_ret(rpc_ret_data);\r\n"
            "    rpc_stream_ret.setVersion(QDataStream::Qt_4_8);\r\n"
            "    rpc_stream_ret >> %2rpc_temp_ret;\r\n"
            "    return rpc_temp_ret;\r\n";


    retLine = retLine.arg(retDef, (isPointer ? "*" : ""));

    return retLine;
}

QString QtRPCGenerator::createReturnTypeDef(const QString &retArgText, bool *isPointer, QString *typePure)
{
    if (retArgText.trimmed() == "void")
        return QString();
    bool isPointerType = false;

    QString typeText = retArgText.trimmed();
    if (typeText.mid(0, 5) == "const")
    {
        typeText.remove(0, 5);//remove const
        typeText = typeText.trimmed();
    }

    if (typeText.right(1) == "*")
    {
        typeText.remove(typeText.length() - 1, 1); //remove '*'
        typeText = typeText.trimmed();
        isPointerType = true;
    }
    else
    {

    }

    if (typePure != 0)
        *typePure = typeText;

    if (isPointer != 0)
        *isPointer = isPointerType;

    QString pointerTypeCheck = isPointerType ? (typeText + " *") : typeText;
    QString pointerNewCheck = isPointerType ? ("= new " + typeText + "()") : "";

    QString retLine = "    %1 rpc_temp_ret %2;";


    return retLine.arg(pointerTypeCheck, pointerNewCheck);
}

QString QtRPCGenerator::createInputContent(const QString &inArgs)
{
    QString argBody =
            "    QByteArray rpc_arg_data;\r\n"
            "    QDataStream rpc_stream_arg(&rpc_arg_data, QIODevice::WriteOnly);\r\n"
            "    rpc_stream_arg.setVersion(QDataStream::Qt_4_8);\r\n"
            "    rpc_stream_arg%1;\r\n";


    return argBody.arg(inArgs);
}

QString QtRPCGenerator::createFunctionContent(const QString &funcSignature, const QString &retArgText,
                                              const QString &inArgs, const QString &unique)
{
    QString argInput = createInputContent(inArgs);
    QString argReturn = createReturnContent(retArgText);

    QString loop = "    RpcEventLoop loop(%1);\r\n"
                   "    QObject::connect((QObject*)d_ptr, SIGNAL(doGotMsg(int,QByteArray,int)),\r\n"
                   "                     &loop, SLOT(rpcExit(int,QByteArray,int)));\r\n"
                   "    QObject::connect((QObject*)d_ptr, SIGNAL(doExit()),\r\n"
                   "                     &loop, SLOT(quit()));\r\n"
                   "    writeMsg(%2, rpc_arg_data, loop.descriptor());\r\n";

    return funcSignature + "\r\n{\r\n" + argInput + loop.arg(unique, unique) + argReturn + "}\r\n\r\n";
}

QString QtRPCGenerator::createCallbackOutputContent(const QString &unique, const QString &argDef,
                                                    const QString &outArg, const QString &inArgVoid,
                                                    const QString &retTypeText, const QString &invokeArg)
{
    QString retContent = "        case %1:\r\n"
                         "        {\r\n            %2\r\n"
                         "            %3\r\n"
                         "            QList<void*> void_args;\r\n"
                         "            %4\r\n"
                         "        %5\r\n"
                         "            %6\r\n"
                         "            %7\r\n"
                         "            break;\r\n"
                         "        }\r\n";
    QString argVoidInputLine =  "void_args%1;\r\n";
    QString argOutLine = "rpc_stream_arg%1;\r\n";

    bool isPointer = false;
    QString retTypePure;
    QString retDef = createReturnTypeDef(retTypeText, &isPointer, &retTypePure);
    QString retName = "%1rpc_temp_ret";
    QString sendbackData =
            "QByteArray data_send_back;\r\n"
            "            QDataStream stream_send_back(&data_send_back, QIODevice::WriteOnly);\r\n"
            "            stream_send_back << %1;\r\n"
            "            writeMsg(unique, data_send_back, descriptor);\r\n";

    if (argDef.isEmpty())
    {
        argVoidInputLine.clear();
        argOutLine.clear();
    }
    else
    {
        argVoidInputLine = argVoidInputLine.arg(inArgVoid);
        argOutLine = argOutLine.arg(outArg);
    }
    if (retDef.isEmpty())
    {
        retName.clear();
        sendbackData.clear();
    }
    else
    {
        sendbackData = sendbackData.arg(retName.arg(isPointer ? "*" : ""));
        retName = ", " + retName.arg(isPointer ? "" : "&");
    }

    QString invokeArgContent = createFuncInvokeContent(unique, retTypePure,
                                                       QString(isPointer ? "*rpc_temp_ret" : "rpc_temp_ret"),
                                                       invokeArg);

    return retContent.arg(unique, argDef, argOutLine, argVoidInputLine, retDef, invokeArgContent, sendbackData);
}

QString QtRPCGenerator::createFuncInvokeContent(const QString &unique, const QString &invokeRetType,
                                                const QString &invokeRetName, const QString &invokeIn)
{
    QString invokeContent =
            "QMetaObject::invokeMethod(this->object(%1),"
            "this->slotName(%2)%3%4);";


    QString retArg =
            ",QReturnArgument<%1 >(\"%2\", %3)";

    QString inputArg =
            ",%1";

    if (invokeRetType.isEmpty())
        retArg.clear();
    else
        retArg = retArg.arg(invokeRetType, invokeRetType, invokeRetName);

    if (invokeIn.isEmpty())
        inputArg.clear();
    else
        inputArg = inputArg.arg(invokeIn);

    return invokeContent.arg(unique, unique, retArg, inputArg);
}

void QtRPCGenerator::saveToFunctions(const QStringList &headers, const QString &typeSerialFunction, const QString &funcIds, const QString &headerFunctionBody)
{
    const QString dependComment("// ==================depend headers==================\r\n");
    const QString defineFuncId("// ================== function ids  ==================\r\n");
    const QString operatorFuncs("// ================operator functions================\r\n");
    const QString rpcCallFuncs("// ===================rpc functions===================\r\n");

    const QString myEventLoop =
            "#include <QEventLoop>                                               \r\n"
            "class RpcEventLoop : public QEventLoop                              \r\n"
            "{                                                                   \r\n"
            "    Q_OBJECT                                                        \r\n"
            "public:                                                             \r\n"
            "    RpcEventLoop(int id) : id_(id), descriptor_(0){}                \r\n"
            "    ~RpcEventLoop() {}                                              \r\n"
            "    const QByteArray &data()                                        \r\n"
            "    {                                                               \r\n"
            "        this->exec();                                               \r\n"
            "        return data_;                                               \r\n"
            "    }                                                               \r\n"
            "    int descriptor()                                                \r\n"
            "    {                                                               \r\n"
            "        return descriptor_;                                         \r\n"
            "    }                                                               \r\n"
            "                                                                    \r\n"
            "public slots:                                                       \r\n"
            "    void rpcExit(int msgId, const QByteArray &data, int descriptor) \r\n"
            "    {                                                               \r\n"
            "        if (msgId != id_)                                           \r\n"
            "            return;                                                 \r\n"
            "        data_ = data;                                               \r\n"
            "        descriptor_ = descriptor;                                   \r\n"
            "        QEventLoop::exit();                                         \r\n"
            "    }                                                               \r\n"
            "                                                                    \r\n"
            "private:                                                            \r\n"
            "    int id_;                                                        \r\n"
            "    int descriptor_;                                                \r\n"
            "    QByteArray data_;                                               \r\n"
            "};                                                                  \r\n"
            "#include \"%1_rpc_functions.moc\"                                   \r\n";

    QString fileContent = /*QString::fromWCharArray(L"//xairy.com© Co., Ltd.\r\n")*/
            /*+*/ dependComment + headers.join("\r\n") + "\r\n\r\n\r\n"
            + defineFuncId + funcIds + "\r\n\r\n\r\n"
            + operatorFuncs + typeSerialFunction
            + "\r\n" + myEventLoop.arg(QFileInfo(headerFileName_).baseName()) + "\r\n"
            + rpcCallFuncs + headerFunctionBody;

    QString desFile = QFileInfo(headerFileName_).baseName() + "_rpc_functions.cpp";
    QDir dir(filePath_);
    if (dir.exists(filePath_))
    {
        desFile = dir.filePath(desFile);
    }

    QFile file(desFile.toLower());
    if (!file.open(QIODevice::WriteOnly))
    {
        qCritical() << "generate file failed.";
        return;
    }

    QTextStream stream(&file);
    stream.setCodec(QTextCodec::codecForName("utf-8"));
    stream.setGenerateByteOrderMark(true);
    stream << fileContent;
    file.close();
}

void QtRPCGenerator::saveToCallbacks(const QString &typeSerialFunction, const QString &switchContent,
                                     const QStringList &headers, const QString &funcIds)
{
    QString fileContent = "#ifndef %1\r\n"
                          "#define %2\r\n\r\n"
                          "#include \"QtRPCManager.h\"\r\n"
                          "%3\r\n"
                          "%4\r\n"
                          "class %5 : public QtRPCItem\r\n"
                          "{\r\n"
                          "public:\r\n"
                          "    explicit %6(SocketType stype = SocketTcp, ConnectionType ctype = ConnectionPeer,\r\n"
                          "                      const QString &peerId = QString()) : QtRPCItem(stype, ctype, peerId){}\r\n"
                          "    explicit %6(QtRPCNetwork *network) : QtRPCItem(network){}\r\n\r\n"
                          "    virtual void setMsgData(int unique, const QByteArray &data_rpc_in, int descriptor = 0)\r\n"
                          "    {\r\n"
                          "        QDataStream rpc_stream_arg(data_rpc_in);\r\n"
                          "        rpc_stream_arg.setVersion(QDataStream::Qt_4_8);\r\n"
                          "        %7"
                          "    }\r\n"
                          "};\r\n\r\n"
                          "#endif //%8";

    QString desFile = QFileInfo(headerFileName_).baseName() + "_rpc_callbacks.h";
    QString headerMacro = desFile.toUpper().replace(".", "_");
    QDir dir(filePath_);
    if (dir.exists(filePath_))
    {
        desFile = dir.filePath(desFile);
    }

    QFile file(desFile.toLower());
    if (!file.open(QIODevice::WriteOnly))
    {
        qCritical() << "generate file failed.";
        return;
    }

    QTextStream stream(&file);
    stream.setCodec(QTextCodec::codecForName("utf-8"));
    stream.setGenerateByteOrderMark(true);

    stream << fileContent.arg(headerMacro, headerMacro, funcIds, headers.join("\r\n") + "\r\n" + typeSerialFunction,
                              headerClassName_, headerClassName_, switchContent, headerMacro);
    file.close();
}
