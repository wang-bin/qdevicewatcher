# qmake library building template pri file
# Copyright (C) 2011 Wang Bin <wbsecg1@gmail.com>
# Shanghai, China.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
############################## HOW TO ##################################
# Suppose the library name is XX
# Usually what you need to change are: staticlink, LIB_VERSION, NAME and DLLDESTDIR.
# And rename xx-buildlib and LIBXX_PRI_INCLUDED
# the contents of libXX.pro is:
#
#    TEMPLATE = lib
#    QT -= gui
#    CONFIG *= xx-buildlib
#    include(libXX.pri)
#    HEADERS = ...
#    SOURCES = ...
#    ...
# the content of other pro using this library is:
#
#    TEMPLATE = app
#    include(dir_of_XX/libXX.pri)
#    HEADERS = ...
#    SOURCES = ...
#


!isEmpty(LIBQDEVICEWATCHER_PRI_INCLUDED):error("libQDeviceWatcher.pri already included")
LIBQDEVICEWATCHER_PRI_INCLUDED = 1

staticlink = 0  #1 or 0. use static lib or not
LIB_VERSION = 2.0.0
#QT += network

NAME = QDeviceWatcher
TEMPLATE += fakelib
PROJECT_TARGETNAME = $$qtLibraryTarget($$NAME)
TEMPLATE -= fakelib


include(../common.pri)
#load(../common.pri)
CONFIG += depend_includepath #?

PROJECT_SRCPATH = $$PWD
PROJECT_LIBDIR = $$qtLongName($$PWD/../lib)
#PROJECT_LIBDIR = $$PWD/../bin #for win dll

INCLUDEPATH += $$PROJECT_SRCPATH
DEPENDPATH += $$PROJECT_SRCPATH
QMAKE_LFLAGS_RPATH += #will append to rpath dir

!qdevicewatcher-buildlib {

	#The following may not need to change
	CONFIG *= link_prl
	LIBS += -L$$PROJECT_LIBDIR -l$$qtLibName($$NAME)
	isEqual(staticlink, 1) {
		PRE_TARGETDEPS += $$PROJECT_LIBDIR/$$qtStaticLib($$NAME)
	} else {
		win32 {
			PRE_TARGETDEPS += $$PROJECT_LIBDIR/$$qtSharedLib($$NAME, $$LIB_VERSION)
		} else {
			PRE_TARGETDEPS += $$PROJECT_LIBDIR/$$qtSharedLib($$NAME)
			unix: QMAKE_RPATHDIR += $$DESTDIR:$$PROJECT_LIBDIR #executable's dir
		}
	}
} else {
	#Add your additional configuration first
	win32: LIBS += -lUser32


	#The following may not need to change

	#TEMPLATE = lib
	VERSION = $$LIB_VERSION
	TARGET = $$PROJECT_TARGETNAME
	DESTDIR= $$PROJECT_LIBDIR

	CONFIG *= create_prl #
	isEqual(staticlink, 1) {
		CONFIG -= shared dll ##otherwise the following shared is true, why?
		CONFIG *= staticlib
	}
	else {
		DEFINES += Q_DLL_LIBRARY #win32-msvc*
		CONFIG *= shared #shared includes dll
	}

	shared {
		DLLDESTDIR = ../bin #copy shared lib there
		CONFIG(release, debug|release):
			!isEmpty(QMAKE_STRIP): QMAKE_POST_LINK = -$$QMAKE_STRIP $$PROJECT_LIBDIR/$$qtSharedLib($$NAME)

		#copy from the pro creator creates.
		symbian {
			MMP_RULES += EXPORTUNFROZEN
			TARGET.UID3 = 0xE4CC8061
			TARGET.CAPABILITY =
			TARGET.EPOCALLOWDLLDATA = 1
			addFiles.sources = $$qtSharedLib($$NAME, $$LIB_VERSION)
			addFiles.path = !:/sys/bin
			DEPLOYMENT += addFiles
		}
	}
	unix:!symbian {
		maemo5 {
			target.path = /opt/usr/lib
		} else {
			target.path = /usr/lib
		}
		INSTALLS += target
	}

}

unset(LIB_VERSION)
unset(PROJECT_SRCPATH)
unset(PROJECT_LIBDIR)
unset(PROJECT_TARGETNAME)
unset(staticlink)
