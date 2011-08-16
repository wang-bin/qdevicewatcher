#-------------------------------------------------
#
# Project created by QtCreator 2011-07-31T22:41:06
#
#-------------------------------------------------

QT       += core
#QT		 += network
QT       -= gui

TARGET = QUsbMonitor
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

include(config.pri)


unix {
	SOURCES += qusbwatcher_linux.cpp
}
SOURCES += main.cpp \
	qusbwatcher.cpp

HEADERS += \
    qusbwatcher.h

OTHER_FILES += \
    TODO.txt
