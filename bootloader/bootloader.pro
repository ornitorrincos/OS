TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /usr/include/efi

SOURCES += main.c misc.s \
    disk.c \
    ELF.c \
    vga.c \
    vmmem.c \
    memory.c \
    paging.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    ELF.h \
    disk.h \
    vga.h \
    vmmem.h \
    memory.h \
    memorytypes.h \
    paging_struct.h \
    paging.h

