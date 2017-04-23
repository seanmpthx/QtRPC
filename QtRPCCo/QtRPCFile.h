#ifndef QTRPCFILE_H
#define QTRPCFILE_H

#include <QFile>
#include <QHash>
#include <QStringList>
#include "QtRPCClass.h"
#include "types.h"

class QtRPCFile
{
public:
    explicit QtRPCFile(const QString &fileName = QString());
    void  setFile(const QString &fileName);
    bool  isValid() const;
    const QString &fileName() const;

    int parse();

    void getFuncMembersItems(QHash<QString, ClassInfo> *classInfo);

public:
    int procHeader(const QStringRef &ref);


private:
    void dumpKwis();
    void prepareKeyword();
    int  toNextChar1(const QStringRef &ref, QChar ch, int offset = 0);
    int  toNextChar2(const QStringRef &ref, QChar ch1, QChar ch2, int offset = 0);
    int  checkNumber(const QStringRef &ref, QChar ch, QString &word);
    KeyWords isKeyword(const QString &word);

    void ExtractClass();
    int  ExtractClass(int index, int isClassOrStruct = true);
    int  beginWithBraceLeft(int index);

    void createClassParser(int posStart, int posEnd,
                           bool isClassOrStruct,
                           const QString &className,
                           const QString &baseClass = QString());
    void startClassContentParse();

private:
    QString fileName_;
    QString fileContent_;

    QList<KeyWordItem> kwis_;
    QHash<QString, KeyWords> keyType_;

    //results
    QList<QtRPCClass> classes_;
    QStringList headerIncludes;
};

#endif // QTRPCFILE_H
