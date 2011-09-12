TEMPLATE = app
#QT += network
CONFIG   += console
CONFIG   -= app_bundle

TARGET = hotplugwatcher

include(../config.pri)
DESTDIR=$$replace(DESTDIR, $${DESTDIR}, ../$${DESTDIR})

LIBS += -L../lib -lQDeviceWatcher
win32:LIBS += -lUser32
INCLUDEPATH += ../src

SOURCES += main.cpp
HEADERS += hotplugwatcher.h

