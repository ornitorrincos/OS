TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    bootloader_compat.cpp \
    paging.c

HEADERS += \
    bootloader_compat.h \
    paging_struct.h \
    paging.h

