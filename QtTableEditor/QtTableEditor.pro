#-------------------------------------------------
#
# Project created by QtCreator 2015-09-03T20:55:05
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = visual_stats
TEMPLATE = app


SOURCES += src/main.cpp\
        src/mainwindow.cpp \
    src/statstablemodel.cpp \
    src/statskeyvaluemodel.cpp \
    src/statsserializer.cpp \
    src/statsdocument.cpp \
    src/aboutdialog.cpp \
    src/insertrowdialog.cpp \
    src/customtableview.cpp

HEADERS  += src/mainwindow.h \
    src/statstablemodel.h \
    src/basekeyvaluemodel.h \
    src/statskeyvaluemodel.h \
    src/statsserializer.h \
    src/statsdocument.h \
    src/istatsmodelprovider.h \
    src/aboutdialog.h \
    src/insertrowdialog.h \
    src/customtableview.h

FORMS    += src/mainwindow.ui \
    src/aboutdialog.ui \
    src/insertrowdialog.ui
