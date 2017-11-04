include(toolchain.pri)
include(build.pri)

DEFINES += QT_NO_PRINTER

QT += core gui network script
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

TARGET = panel

HEADERS += \
    src/mainwindow.h \
    src/qcustomplot.h \
    src/script.h \
    src/utils.h \
    src/driver.h \
    src/tag.h \
    src/ini.h \
    src/config.h

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/qcustomplot.cpp \
    src/script.cpp \
    src/utils.cpp \
    src/driver.cpp \
    src/tag.cpp \
    src/config.cpp

FORMS += \
    src/mainwindow.ui

contains(PLATFORM, arm) {
    INSTALLS += target
        target.path += /opt

    INSTALLS += conf
        conf.path = /opt
        conf.files += $$PWD/install/panel/panel.sh
        conf.files += $$PWD/install/panel/config.ini
        conf.files += $$PWD/install/panel/tags.efs
        conf.files += $$PWD/install/panel/test1.script
        conf.files += $$PWD/install/panel/test2.script
        conf.files += $$PWD/install/panel/test3.script

    DEFINES += ARM
}
