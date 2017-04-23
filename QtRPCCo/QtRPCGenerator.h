#ifndef QTRPCGENERATOR_H
#define QTRPCGENERATOR_H

#include <QHash>
#include "types.h"

struct FunctionId
{
    QString funcName;
    QString signature;
    QString paramIn;
    QString paramOut;
    QString retType;
};

class FunctionIds : public QList<FunctionId>
{
public:
    QString createFunctionIds();

};

class QtRPCGenerator
{
public:
    QtRPCGenerator();

    void setHeaderPath(const QString &path);
    void setHeaderFile(const QString &fileName);
    void setHeaderInfo(const QHash<QString, ClassInfo> &info);
    void setRefrenceInfo(const QHash<QString, ClassInfo> &info);
    bool startGenerate();

private:
    QPair<QString, QString> createRpcFuncContent(QString *functionId, QStringList *depends, QStringList *headers);
    QString createTypeFunction(const QStringList &types, QStringList *headers);

    void pushDepends(const QStringList &dependsIn, QStringList *dependsOut);
    QString pushFunctionId(const QString &funcSign, const QString &name, QString *funcId);
    void pushHeaders(const QString &header, QStringList *headers);
    QString createReturnContent(const QString &retArgText);
    QString createReturnTypeDef(const QString &retArgText, bool *isPointer = 0, QString *typePure = 0);
    QString createInputContent(const QString &inArgs);
    QString createFunctionContent(const QString &funcSignature, const QString &retArgText,
                                  const QString &inArgs, const QString &unique);
    QString createCallbackOutputContent(const QString &unique, const QString &argDef,
                                        const QString &outArg, const QString &inArgVoid,
                                        const QString &retTypeText, const QString &invokeArg);
    QString createFuncInvokeContent(const QString &unique, const QString &invokeRetType,
                                    const QString &invokeRetName, const QString &invokeIn);

    void saveToFunctions(const QStringList &headers, const QString &typeSerialFunction,
                         const QString &funcIds, const QString &headerFunctionBody);
    void saveToCallbacks(const QString &typeSerialFunction, const QString &switchContent,
                         const QStringList &headers, const QString &funcIds);

private:
    QString filePath_;
    QString headerFileName_;
    QString headerClassName_;
    int funcIndex_;
    QList<FuncItem> headerFunctions_;

    QHash<QString, ClassInfo> refrenceMembers_;
};

#endif // QTRPCGENERATOR_H
