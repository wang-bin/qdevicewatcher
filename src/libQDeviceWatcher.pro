TEMPLATE = lib
QT -= gui
CONFIG *= qdevicewatcher-buildlib

!include(libQDeviceWatcher.pri): error(could not find ibQDeviceWatcher.pri)

#src
unix:  SOURCES += qdevicewatcher_linux.cpp
win32 {
	wince*: SOURCES += qdevicewatcher_wince.cpp
	else:  SOURCES += qdevicewatcher_win.cpp
}

SOURCES += qdevicewatcher.cpp \
	qdevicechangeevent.cpp

HEADERS += \
	qdevicewatcher_p.h \
	qdevicewatcher.h \
	qdevicechangeevent.h \
	qdevicewatcher_global.h


