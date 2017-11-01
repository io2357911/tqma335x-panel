include(toolchain.pri)
include(build.pri)

DEFINES += QT_NO_PRINTER

QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = counter

HEADERS += \
    mainwindow.h \
    qcustomplot.h

SOURCES += main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp

FORMS += \
    mainwindow.ui

contains(PLATFORM, arm) {
    INSTALLS += target
        target.path += /opt
}
