######################################################################
# Automatically generated by qmake (3.0) Wed May 28 14:19:56 2014
######################################################################

QT += opengl widgets
CONFIG += c++11
QMAKE_CXXFLAGS += -W -Wall -g
LIBS += -lglu32
TEMPLATE = app
TARGET = a2
INCLUDEPATH += .

# Input
HEADERS += AppWindow.hpp Viewer.hpp 
SOURCES += main.cpp AppWindow.cpp Viewer.cpp 

DISTFILES += \
    shader.frag \
    shader.vert
