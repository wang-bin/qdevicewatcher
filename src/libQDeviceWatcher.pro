TEMPLATE = lib
QT -= gui
CONFIG *= staticlib qdevicewatcher-buildlib

include(libQDeviceWatcher.pri)
DESTDIR=$$PROJECT_LIBDIR
TARGET = $$QDEVICEWATCHER_NAME
