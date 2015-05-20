TEMPLATE = lib
QT -= gui
CONFIG *= qdevicewatcher-buildlib
staticlib: DEFINES += BUILD_QDEVICEWATCHER_STATIC

!include(libQDeviceWatcher.pri): error(could not find ibQDeviceWatcher.pri)

unix {
  macx {
    SOURCES += qdevicewatcher_mac.cpp
    LIBS += -framework DiskArbitration -framework Foundation
  } else {
    SOURCES += qdevicewatcher_linux.cpp
  }
}
win32 {
  wince*: SOURCES += qdevicewatcher_wince.cpp
  else:  SOURCES += qdevicewatcher_win32.cpp
}

SOURCES += qdevicewatcher.cpp \
        qdevicechangeevent.cpp


HEADERS += \
	qdevicewatcher_p.h \
	qdevicewatcher.h \
	qdevicechangeevent.h \
	qdevicewatcher_global.h


