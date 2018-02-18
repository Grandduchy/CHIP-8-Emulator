TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    src/Chip8.cpp \
    src/main.cpp

HEADERS += \
    include/Chip8.hpp


INCLUDEPATH += -I /home/joshua/Development/Git/CHIP-8/include
