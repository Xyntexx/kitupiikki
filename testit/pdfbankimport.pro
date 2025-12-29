QT += pdf

CONFIG += c++14

CONFIG += qt console
CONFIG -= app_bundle

# DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += KITSAS_DEBUG

TEMPLATE = app

INCLUDEPATH += $$PWD/../kitsas
VPATH += $$PWD/../kitsas

# Include PDF import functionality
include(../kitsas/pdftuonti.pri)

SOURCES += \
    pdfbankimport.cpp

TARGET = pdfbankimport
