QT += core network
QT -= gui

CONFIG(debug, debug|release){
TARGET = QtRPCd
} else {
TARGET = QtRPCr
}

DESTDIR = ../bin

TEMPLATE = lib

SOURCES += \
    ../source/RPC/QtRPCItem.cpp \
    ../source/RPC/QtRPCItemPrivate.cpp \
    ../source/RPC/QtRPCManager.cpp \
    ../source/RPC/QtRPCManagerPrivate.cpp \
    ../source/LAN/LanSearcher.cpp \
    ../source/LAN/MsgSection.cpp \
    ../source/LAN/QtRPCNetworkLocal.cpp \
    ../source/LAN/QtRPCNetworkLocalPrivate.cpp \
    ../source/LAN/QtRPCNetworkTcp.cpp \
    ../source/LAN/QtRPCNetworkTcpPrivate.cpp

HEADERS += \
    ../include/QtRPCManager.h \
    ../include/QtRPCNetwork.h \
    ../source/RPC/QtRPCItemPrivate.h \
    ../source/RPC/QtRPCManagerPrivate.h \
    ../source/LAN/LanSearcher.h \
    ../source/LAN/MsgSection.h \
    ../source/LAN/QtRPCNetworkLocal.h \
    ../source/LAN/QtRPCNetworkLocalPrivate.h \
    ../source/LAN/QtRPCNetworkTcp.h \
    ../source/LAN/QtRPCNetworkTcpPrivate.h
