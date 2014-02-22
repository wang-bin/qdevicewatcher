TEMPLATE = lib
QT -= gui
CONFIG *= qdevicewatcher-buildlib

!include(libQDeviceWatcher.pri): error(could not find ibQDeviceWatcher.pri)

#src
unix:  SOURCES += qdevicewatcher_linux.cpp
win32 {
	wince*: SOURCES += qdevicewatcher_wince.cpp
	else:  SOURCES += qdevicewatcher_win32.cpp
}
macx {
    LIBS += -framework DiskArbitration -framework Foundation
}

SOURCES += qdevicewatcher.cpp \
	qdevicechangeevent.cpp \
    qdevicewatcher_mac.cpp

HEADERS += \
	qdevicewatcher_p.h \
	qdevicewatcher.h \
	qdevicechangeevent.h \
	qdevicewatcher_global.h


