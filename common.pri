#Copyright (C) 2011 Wang Bin <wbsecg1@gmail.com>
#Shanghai, China.
#GPL v2

CONFIG += #ezx#static ezx
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
	QMAKE_CXXFLAGS.ARMCC +=
}

#*arm*: ARCH_EXT = $${ARCH_EXT}_arm
#isEqual(QT_ARCH, arm) {
contains(QT_ARCH, arm.*) {
	ARCH_EXT = $${ARCH_EXT}_$${QT_ARCH}
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
defineReplace(qtLibraryName) {
	unset(LIBRARY_NAME)
	LIBRARY_NAME = $$1
	CONFIG(debug, debug|release) {
		!debug_and_release|build_pass {
			mac:RET = $$member(LIBRARY_NAME, 0)_debug
			else:win32:RET = $$member(LIBRARY_NAME, 0)d
		}
	}
	isEmpty(RET):RET = $$LIBRARY_NAME
	return($$RET)
}


#fakelib
defineReplace(qtStaticLib) {
	unset(LIB_FULLNAME)
	TEMPLATE += fakelib
	LIB_FULLNAME = $$qtLibraryTarget($$1)
	TEMPLATE -= fakelib
	*msvc*: LIB_FULLNAME = $$member(LIB_FULLNAME, 0).lib
	else: LIB_FULLNAME = lib$$member(LIB_FULLNAME, 0).a
	return($$LIB_FULLNAME)
}

defineReplace(qtSharedLib) {
	unset(LIB_FULLNAME)
	TEMPLATE += fakelib
	LIB_FULLNAME = $$qtLibraryTarget($$1)
	TEMPLATE -= fakelib
	win32: LIB_FULLNAME = $$member(LIB_FULLNAME, 0).dll
	else: LIB_FULLNAME = lib$$member(LIB_FULLNAME, 0).so
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
}
else: DESTDIR = $$BUILD_DIR/lib

OBJECTS_DIR = $$qtLongName($$BUILD_DIR/.obj/)
 #for Qt2, Qt3 which does not have QT_VERSION. Qt4: $$[QT_VERSION]
MOC_DIR = $$BUILD_DIR/.moc/$${QT_VERSION}
RCC_DIR = $$BUILD_DIR/.rcc/$${QT_VERSION}
UI_DIR  = $$BUILD_DIR/.ui/$${QT_VERSION}

#unix: QMAKE_POST_LINK=strip $(TARGET)
!build_pass:message(target: $$DESTDIR/$$TARGET)

#before target name changed
TRANSLATIONS += i18n/$${TARGET}_zh-cn.ts #i18n/$${TARGET}_zh_CN.ts
