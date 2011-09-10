TEMPLATE = lib
#QT += network
QT -= gui

TARGET = QDeviceWatcher

include(../config.pri)
DESTDIR=$$replace(DESTDIR, $${DESTDIR}, ../$${DESTDIR})

unix {
    SOURCES += qdevicewatcher_linux.cpp
} else:win32 {
        SOURCES += qdevicewatcher_win.cpp
}

SOURCES += qdevicewatcher.cpp

HEADERS += \
    qdevicewatcher_p.h \
    qdevicewatcher.h
