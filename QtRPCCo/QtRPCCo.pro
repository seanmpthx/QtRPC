QT += core
QT -= gui

TARGET = QtRPCCo
#CONFIG += console
#CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    QtRPCCo.cpp \
    QtRPCFile.cpp \
    QtRPCClass.cpp \
    QtRPCMember.cpp \
    QtRPCFunction.cpp \
    QtRPCGenerator.cpp

HEADERS += \
    QtRPCCo.h \
    QtRPCFile.h \
    QtRPCClass.h \
    QtRPCMember.h \
    types.h \
    QtRPCFunction.h \
    QtRPCGenerator.h

