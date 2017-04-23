#-------------------------------------------------
#
# Project created by QtCreator 2017-04-10T23:33:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RpcItemTest2
TEMPLATE = app


CONFIG(debug, debug|release){
LIBS += -L../QtRPC/bin/ -lQtRPCd
} else {
LIBS += -L../QtRPC/bin/ -lQtRPCr
}
INCLUDEPATH += ../QtRPC/include

SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h \
    rcpx_rpc_callbacks.h \
    rpc_types.h

FORMS    += widget.ui
