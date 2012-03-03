TEMPLATE = app
QT		 -= gui
CONFIG   += console
CONFIG   -= app_bundle

TARGET = hotplugwatcher
include(../src/libQDeviceWatcher.pri)

win32:!wince*:LIBS += -lUser32

SOURCES += main.cpp
HEADERS += hotplugwatcher.h

