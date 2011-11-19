TEMPLATE = lib
QT -= gui
CONFIG *= qdevicewatcher-buildlib

include(libQDeviceWatcher.pri)

#src
unix: SOURCES += qdevicewatcher_linux.cpp
else:win32: SOURCES += qdevicewatcher_win.cpp

SOURCES += qdevicewatcher.cpp \
	qdevicechangeevent.cpp

HEADERS += \
	qdevicewatcher_p.h \
	qdevicewatcher.h \
	qdevicechangeevent.h \
	qdevicewatcher_global.h


