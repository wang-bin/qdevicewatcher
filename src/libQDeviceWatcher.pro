TEMPLATE = lib
#QT += network
QT -= gui
CONFIG += staticlib

TARGET = QDeviceWatcher

include(../config.pri)
DESTDIR=$$replace(DESTDIR, $${DESTDIR}, ../$${DESTDIR})

unix {
    SOURCES += qdevicewatcher_linux.cpp
} else:win32 {
    LIBS += -lUser32
    SOURCES += qdevicewatcher_win.cpp
}

SOURCES += qdevicewatcher.cpp

HEADERS += \
    qdevicewatcher_p.h \
    qdevicewatcher.h
