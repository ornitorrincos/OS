TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += /usr/include/efi

SOURCES += main.c misc.s

include(deployment.pri)
qtcAddDeployment()

