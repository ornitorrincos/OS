TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /usr/include/efi

SOURCES += main.c misc.s \
    disk.c \
    ELF.c \
    vga.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    ELF.h \
    disk.h \
    vga.h

