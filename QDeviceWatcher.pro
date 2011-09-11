TEMPLATE = subdirs
SUBDIRS = libqdevicewatcher test

libqdevicewatcher.file = src/libQDeviceWatcher.pro
test.file = test/watcher.pro
test.depends += libqdevicewatcher


OTHER_FILES += \
    TODO.txt \
    README
















