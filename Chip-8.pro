TEMPLATE = app
TARGET = Chip-8

QT       += core gui

CONFIG += c++14

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
    Chip8.cpp \
    mainwindow.cpp

HEADERS += \
    Chip8.hpp \
    mainwindow.hpp

FORMS += \
    mainwindow.ui
