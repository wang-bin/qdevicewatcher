include(../common.pri)
#load(../common.pri)
#QT += network

QDEVICEWATCHER_SRCPATH = $$PWD
PROJECT_LIBDIR = $$PWD/../lib$${PLATFORM_EXT}$${ARCH_EXT}$${TOOLCHAIN_EXT}

TEMPLATE += fakelib
QDEVICEWATCHER_NAME = $$qtLibraryTarget(QDeviceWatcher)
TEMPLATE -= fakelib

INCLUDEPATH += $$QDEVICEWATCHER_SRCPATH
DEPENDPATH += $$QDEVICEWATCHER_SRCPATH

!qdevicewatcher-buildlib {
	PRE_TARGETDEPS += $$PROJECT_LIBDIR/$$qtStaticLib(QDeviceWatcher)
	LIBS += -L$$PROJECT_LIBDIR  -l$$QDEVICEWATCHER_NAME
} else {
#src
	unix {
		SOURCES += qdevicewatcher_linux.cpp
	} else:win32 {
		LIBS += -lUser32
		SOURCES += qdevicewatcher_win.cpp
	}

	SOURCES += qdevicewatcher.cpp \
		qdevicechangeevent.cpp

	HEADERS += \
		qdevicewatcher_p.h \
		qdevicewatcher.h \
		qdevicechangeevent.h

}
