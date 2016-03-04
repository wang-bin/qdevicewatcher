TEMPLATE = lib
QT -= gui
CONFIG *= qdevicewatcher-buildlib
!include(libQDeviceWatcher.pri): error(could not find ibQDeviceWatcher.pri)
staticlib|isEqual(STATICLINK, 1): DEFINES += BUILD_QDEVICEWATCHER_STATIC

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
  LIBS *= -luser32
}

SOURCES += qdevicewatcher.cpp


HEADERS += \
	qdevicewatcher_p.h \
	qdevicewatcher.h


