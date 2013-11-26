#-------------------------------------------------
#
# Project created by QtCreator 2013-11-25T12:35:25
#
#-------------------------------------------------

QT       += network testlib

TARGET = tst_guitest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += tst_guitest.cpp \
    ../client/clientgui.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"


HEADERS += \
    ../client/clientgui.h
