#include <QCoreApplication>
#include <QStringList>
#include <QTextCodec>
#include "QtRPCCo.h"

#include <QList>
#include <QDataStream>


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("utf-8");

    QTextCodec::setCodecForLocale(codec);
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    QTextCodec::setCodecForTr(codec);
    QTextCodec::setCodecForCStrings(codec);
#endif


    QStringList args = a.arguments();
    args.pop_front();

    QtRPCCo rpcc(args);


    return 0;
}

