TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.c \
    elfinfoprint.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    datatypes.h \
    elf64header.h \
    elfinfoprint.h

