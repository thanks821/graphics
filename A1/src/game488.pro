######################################################################
# Automatically generated by qmake (3.0) Wed May 21 23:56:48 2014
######################################################################

QT += opengl widgets
CONFIG += c++11
QMAKE_CXXFLAGS += -W -Wall -g
TEMPLATE = app
TARGET = game488
INCLUDEPATH += .

# Input
HEADERS += AppWindow.hpp Viewer.hpp game.hpp
SOURCES += main.cpp AppWindow.cpp Viewer.cpp game.cpp

DISTFILES += \
    shader.frag \
    shader.vert
