#ifndef QTRPCCO_H
#define QTRPCCO_H

#include <QStringList>
#include "QtRPCFile.h"
#include "QtRPCGenerator.h"

class QtRPCCo
{
public:
    QtRPCCo(const QStringList &args);

private:
    bool loadAllCode(const QStringList &args);
    bool startParse();
    bool startGenerate();

private:
    QtRPCFile headerRPCFile_;
    QList<QtRPCFile> codeFiles_;
};

#endif // QTRPCCO_H
