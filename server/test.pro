# -------------------------------------------------
# Project created by QtCreator 2013-09-03T14:51:47
# -------------------------------------------------
QT += testlib
QT += gui
QT += network
QT += sql
TARGET = testserver
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += testserver.cpp \
    session.cpp \
    chatserver.cpp
HEADERS += \
    session.h \
    chatserver.h 
