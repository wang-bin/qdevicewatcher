!isEmpty(LIBQDEVICEWATCHER_PRI_INCLUDED):error("libQDeviceWatcher.pri already included")
LIBQDEVICEWATCHER_PRI_INCLUDED = 1

include(../common.pri)
#load(../common.pri)
#QT += network
CONFIG += depend_includepath #?

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
		SOURCES += $$QDEVICEWATCHER_SRCPATH/qdevicewatcher_linux.cpp
	} else:win32 {
		LIBS += -lUser32
		SOURCES += $$QDEVICEWATCHER_SRCPATH/qdevicewatcher_win.cpp
	}

	SOURCES += $$QDEVICEWATCHER_SRCPATH/qdevicewatcher.cpp \
		$$QDEVICEWATCHER_SRCPATH/qdevicechangeevent.cpp

	HEADERS += \
		$$QDEVICEWATCHER_SRCPATH/qdevicewatcher_p.h \
		$$QDEVICEWATCHER_SRCPATH/qdevicewatcher.h \
		$$QDEVICEWATCHER_SRCPATH/qdevicechangeevent.h

}
