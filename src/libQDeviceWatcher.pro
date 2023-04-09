TEMPLATE = lib
QT -= gui
CONFIG *= qdevicewatcher-buildlib
!include(libQDeviceWatcher.pri): error(could not find ibQDeviceWatcher.pri)
staticlib|isEqual(STATICLINK, 1): DEFINES += BUILD_QDEVICEWATCHER_STATIC

isEqual(Q_DEVICE_WATCHER_DEBUG, 1) {
    DEFINES += CONFIG_DEBUG
}

unix {
  macx {
    SOURCES += qdevicewatcher_mac.cpp
    LIBS += -framework DiskArbitration -framework Foundation
  } else {
    SOURCES += qdevicewatcher_linux.cpp

    greaterThan(QT_MAJOR_VERSION, 5) {
      QT += core5compat
    }
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


