######################################################################
# Automatically generated by qmake (3.0) Wed May 28 16:55:50 2014
######################################################################

QT += opengl widgets
CONFIG += c++11
QMAKE_CXXFLAGS += -W -Wall -g 
#LIBS += -llua5.1
TEMPLATE = app
TARGET = puppeteer
#INCLUDEPATH += /usr/include/lua5.1


LIBS += -L"C:/Program Files (x86)/Lua/5.1/lib" -llua5.1

INCLUDEPATH += "C:/Program Files (x86)/Lua/5.1/include"

#LIBS  += C:/temp/lua-5.1

# Input
HEADERS += algebra.hpp \
           lua488.hpp \
           material.hpp \
           primitive.hpp \
           scene.hpp \
           scene_lua.hpp \
           AppWindow.hpp \
           Viewer.hpp
SOURCES += algebra.cpp \
           main.cpp \
           material.cpp \
           primitive.cpp \
           scene.cpp \
           scene_lua.cpp \
           AppWindow.cpp \ 
           Viewer.cpp

DISTFILES +=
