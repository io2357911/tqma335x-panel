include(toolchain.pri)
include(build.pri)

DEFINES += QT_NO_PRINTER

QT += core gui network script
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = panel

HEADERS += \
    mainwindow.h \
    qcustomplot.h \
    script.h \
    utils.h \
    driver.h \
    tag.h \
    ini.h \
    config.h

SOURCES += main.cpp \
    mainwindow.cpp \
    qcustomplot.cpp \
    script.cpp \
    utils.cpp \
    driver.cpp \
    tag.cpp \
    config.cpp

FORMS += \
    mainwindow.ui

contains(PLATFORM, arm) {
    INSTALLS += target
        target.path += /opt

    DEFINES += ARM
}
