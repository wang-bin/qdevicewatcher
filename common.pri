# qmake common template pri file
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

CONFIG += #ezx
CONFIG += profile
#profiling, -pg is not supported for msvc
debug:!*msvc*:profile {
        QMAKE_CXXFLAGS_DEBUG += -pg
        QMAKE_LFLAGS_DEBUG += -pg
}

#$$[TARGET_PLATFORM]
#$$[QT_ARCH] #windows symbian windowsce arm
PLATFORM_EXT =
ARCH_EXT =
TOOLCHAIN_EXT =
GCC_PREFIX =
unix {
	PLATFORM_EXT = _unix
	*linux*: PLATFORM_EXT = _linux
	*maemo*: PLATFORM_EXT = _maemo
} else:win32 {
	PLATFORM_EXT = _win32
} else:macx {
	PLATFORM_EXT = _macx
}

ezx {
	QT_VERSION = 2.3.8
	CONFIG += qt warn_on release
	DEFINES += QT_THREAD_SUPPORT CONFIG_EZX
	PLATFORM_EXT = _ezx
	isEmpty(QT_ARCH): QT_ARCH = arm
}

#*arm*: ARCH_EXT = $${ARCH_EXT}_arm
contains(QT_ARCH, arm.*) {
	ARCH_EXT = $${ARCH_EXT}_$${QT_ARCH}
	unix: GCC_PREFIX = arm-linux-
}
*64:   ARCH_EXT = $${ARCH_EXT}_x64
*llvm*: TOOLCHAIN_EXT = _llvm
#*msvc*:

win32-msvc* {
        #Don't warn about sprintf, fopen etc being 'unsafe'
        DEFINES += _CRT_SECURE_NO_WARNINGS
}

#################################functions#########################################
defineReplace(cleanPath) {
    win32:1 ~= s|\\\\|/|g
    contains(1, ^/.*):pfx = /
    else:pfx =
    segs = $$split(1, /)
    out =
    for(seg, segs) {
        equals(seg, ..):out = $$member(out, 0, -2)
        else:!equals(seg, .):out += $$seg
    }
    return($$join(out, /, $$pfx))
}

#Acts like qtLibraryTarget. From qtcreator.pri
defineReplace(qtLibName) {
	#TEMPLATE += fakelib
	#LIB_FULLNAME = $$qtLibraryTarget($$1)
	#TEMPLATE -= fakelib
	unset(LIBRARY_NAME)
	LIBRARY_NAME = $$1
	CONFIG(debug, debug|release) {
		!debug_and_release|build_pass {
			mac:RET = $$member(LIBRARY_NAME, 0)_debug
			else:win32:RET = $$member(LIBRARY_NAME, 0)d
		}
	}
	isEmpty(RET):RET = $$LIBRARY_NAME
	!win32: return($$RET)

	isEmpty(2): VERSION_EXT = $$VERSION
	else: VERSION_EXT = $$2
	!isEmpty(VERSION_EXT) {
		VERSION_EXT = $$section(VERSION_EXT, ., 0, 0)
		isEqual(VERSION_EXT, 0):unset(VERSION_EXT)
	}
	RET = $${RET}$${VERSION_EXT}
	unset(VERSION_EXT)
	return($$RET)
}


#fakelib
defineReplace(qtStaticLib) {
	unset(LIB_FULLNAME)
	LIB_FULLNAME = $$qtLibName($$1, $$2)
	*msvc*: LIB_FULLNAME = $$member(LIB_FULLNAME, 0).lib
	else: LIB_FULLNAME = lib$$member(LIB_FULLNAME, 0).a
	return($$LIB_FULLNAME)
}

defineReplace(qtSharedLib) {
	unset(LIB_FULLNAME)
	LIB_FULLNAME = $$qtLibName($$1, $$2)
	win32: LIB_FULLNAME = $$member(LIB_FULLNAME, 0).dll
	else: LIB_FULLNAME = lib$$member(LIB_FULLNAME, 0).so
	#macx: TARGET_BASEPATH = $${TARGET_BASEPATH}.$${QMAKE_EXTENSION_SHLIB} #default_post.prf
	return($$LIB_FULLNAME)
}

defineReplace(qtLongName) {
	unset(LONG_NAME)
	LONG_NAME = $$1$${PLATFORM_EXT}$${ARCH_EXT}$${TOOLCHAIN_EXT}
	return($$LONG_NAME)
}



##############################paths####################################
#message(pwd=$$PWD)			#this file dir
#message(out pwd=$$OUT_PWD)	#Makefile dir
#message(pro file=$$_PRO_FILE_)
#message(pro file pwd=$$_PRO_FILE_PWD_)
BUILD_DIR=$$PWD

isEqual(TEMPLATE, app) {
	DESTDIR = $$BUILD_DIR/bin
	TARGET = $$qtLongName($$TARGET)
	!isEmpty(QMAKE_STRIP): QMAKE_POST_LINK = $$QMAKE_STRIP $$DESTDIR/$${TARGET}* #.exe in win
}
else: DESTDIR = $$BUILD_DIR/lib

OBJECTS_DIR = $$qtLongName($$BUILD_DIR/.obj/)
 #for Qt2, Qt3 which does not have QT_VERSION. Qt4: $$[QT_VERSION]
MOC_DIR = $$BUILD_DIR/.moc/$${QT_VERSION}
RCC_DIR = $$BUILD_DIR/.rcc/$${QT_VERSION}
UI_DIR  = $$BUILD_DIR/.ui/$${QT_VERSION}

!build_pass:message(target: $$DESTDIR/$$TARGET)

#before target name changed
TRANSLATIONS += i18n/$${TARGET}_zh-cn.ts #i18n/$${TARGET}_zh_CN.ts
