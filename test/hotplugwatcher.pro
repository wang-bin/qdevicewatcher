TEMPLATE = app
QT		 -= gui
CONFIG   += console
CONFIG   -= app_bundle

TARGET = hotplugwatcher

Q_DEVICE_WATCHER_DEBUG = 1
cache(Q_DEVICE_WATCHER_DEBUG)
include(../src/libQDeviceWatcher.pri)

win32:!wince*:LIBS += -lUser32

SOURCES += main.cpp
HEADERS += hotplugwatcher.h


contains(MEEGO_EDITION,harmattan) {
    target.path = /opt/hotplugwatcher/bin
    INSTALLS += target
}

maemo5 {
    target.path = /opt/hotplugwatcher/bin
    INSTALLS += target
}
