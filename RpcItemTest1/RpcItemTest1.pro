#-------------------------------------------------
#
# Project created by QtCreator 2017-04-09T18:56:07
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RpcItemTest1
TEMPLATE = app
CONFIG(debug, debug|release){
LIBS += -L../QtRPC/bin/ -lQtRPCd
} else {
LIBS += -L../QtRPC/bin/ -lQtRPCr
}
INCLUDEPATH += ../QtRPC/include


SOURCES += main.cpp\
        widget.cpp \
    rcpx_rpc_functions.cpp

HEADERS  += widget.h \
    rcpx.h \
    rpc_types.h

FORMS    += widget.ui

RESOURCES += \
    testres.qrc
