TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    elfinfoprint.cpp \
    main.cpp \
    bootloader_compat.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    datatypes.h \
    elf64header.h \
    elfinfoprint.h \
    bootloader_compat.h

