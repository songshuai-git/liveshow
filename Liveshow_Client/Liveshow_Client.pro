#-------------------------------------------------
#
# Project created by QtCreator 2019-11-27T19:14:28
#
#-------------------------------------------------

QT       += core gui network
CONFIG   += mobility
MOBILITY += multimedia
QT       += multimedia
QT       += multimediawidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Liveshow_Client
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        Client_w1.cpp \
    ClientHall_w2.cpp \
    ClientRoom_w3.cpp \
    VideoSuface.cpp

HEADERS  += Client_w1.h \
    protocol.h \
    ClientHall_w2.h \
    ClientRoom_w3.h \
    VideoSuface.h

FORMS    += Client_w1.ui \
    ClientHall_w2.ui \
    ClientRoom_w3.ui

RESOURCES += \
    image1.qrc
