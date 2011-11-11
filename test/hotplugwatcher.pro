TEMPLATE = app
CONFIG   += console
CONFIG   -= app_bundle

TARGET = hotplugwatcher
DESTDIR=$$replace(DESTDIR, $${DESTDIR}, ../$${DESTDIR})
QMAKE_LFLAGS_RPATH += $$DESTDIR

win32:LIBS += -lUser32

SOURCES += main.cpp
HEADERS += hotplugwatcher.h

include(../src/libQDeviceWatcher.pri)

