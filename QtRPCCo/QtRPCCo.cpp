#include "QtRPCCo.h"
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QFileInfo>

QtRPCCo::QtRPCCo(const QStringList &args)
{
    if (args.isEmpty())
        exit(1);

    if (!QFile::exists(args.first()))
        exit(2);

    this->headerRPCFile_.setFile(args.first());
    if (!this->headerRPCFile_.isValid())
        exit(3);

    QStringList files = args.count() == 1 ? QStringList() << QFileInfo(args.first()).absolutePath() : args.mid(1);
    if (!this->loadAllCode(files))
        exit(4);

    if (!this->startParse())
        exit(5);

    if (!this->startGenerate())
        exit(6);
}

bool QtRPCCo::loadAllCode(const QStringList &args)
{
    const QStringList suffix =
            QStringList() << "*.h" << "*.hpp" << "*.hxx";

    QString fileHeader = QFileInfo(headerRPCFile_.fileName()).absoluteFilePath();

    QStringList inputFiles;
    foreach(const QString &arg, args)
    {
        QFileInfo info(arg);
        if (info.isDir())
            continue;

        if (!info.exists())
            continue;

        if (!QDir::match(suffix, info.absoluteFilePath()))
            continue;
        if (info.absoluteFilePath() == fileHeader)
            continue;

        QString filePath = info.absoluteFilePath();
        this->codeFiles_.append(QtRPCFile(filePath));
        inputFiles << filePath;
    } //end if

    if (!inputFiles.isEmpty())
    {
        qDebug() << "input files:" << inputFiles;
    }
    return true;
}

bool QtRPCCo::startParse()
{
    for (QList<QtRPCFile> ::iterator itr = this->codeFiles_.begin();
         itr != this->codeFiles_.end(); ++itr)
    {
        QtRPCFile &rpcFile = *itr;
        qDebug() << "start parse," << rpcFile.fileName();
        if (rpcFile.parse() < 0)
            return false;
    }
    if (headerRPCFile_.parse() < 0)
        return false;
    return true;
}

bool QtRPCCo::startGenerate()
{
    QtRPCGenerator generator;
    QHash<QString, ClassInfo> headerInfo;
    headerRPCFile_.getFuncMembersItems(&headerInfo);
    generator.setHeaderInfo(headerInfo);
    generator.setHeaderPath(QFileInfo(headerRPCFile_.fileName()).absolutePath());

    for (QList<QtRPCFile>::iterator itr = codeFiles_.begin(); itr != codeFiles_.end(); ++itr)
    {
        QtRPCFile &file = *itr;

        headerInfo.clear();
        file.getFuncMembersItems(&headerInfo);
        generator.setRefrenceInfo(headerInfo);
    }

    return generator.startGenerate();
}
