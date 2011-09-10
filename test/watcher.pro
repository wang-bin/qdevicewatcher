TEMPLATE = app
CONFIG   += console
CONFIG   -= app_bundle

TARGET = watcher

include(../config.pri)
DESTDIR=$$replace(DESTDIR, $${DESTDIR}, ../$${DESTDIR})

LIBS += -L../lib -lQDeviceWatcher
INCLUDEPATH += ../src
SOURCES += main.cpp
