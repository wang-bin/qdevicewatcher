!isEmpty(LIBQDEVICEWATCHER_PRI_INCLUDED):error("libQDeviceWatcher.pri already included")
LIBQDEVICEWATCHER_PRI_INCLUDED = 1

include(../common.pri)
#load(../common.pri)
#QT += network
CONFIG += depend_includepath #?

QDEVICEWATCHER_SRCPATH = $$PWD
PROJECT_LIBDIR = $$PWD/../lib$${PLATFORM_EXT}$${ARCH_EXT}$${TOOLCHAIN_EXT}

NAME = QDeviceWatcher
TEMPLATE += fakelib
QDEVICEWATCHER_NAME = $$qtLibraryTarget($$NAME)
TEMPLATE -= fakelib

INCLUDEPATH += $$QDEVICEWATCHER_SRCPATH
DEPENDPATH += $$QDEVICEWATCHER_SRCPATH
QMAKE_RPATH += $$PROJECT_LIBDIR

!qdevicewatcher-buildlib {
	PRE_TARGETDEPS += $$PROJECT_LIBDIR/$$qtStaticLib($$NAME)
	LIBS += -L$$PROJECT_LIBDIR  -l$$QDEVICEWATCHER_NAME
	unix: QMAKE_RPATHDIR += $$PROJECT_LIBDIR
} else {
	VERSION = 2.0.0
	TARGET = $$QDEVICEWATCHER_NAME
	DESTDIR= $$PROJECT_LIBDIR
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
