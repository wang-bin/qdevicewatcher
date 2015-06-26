# qmake library building template pri file
# Copyright (C) 2011-2015 Wang Bin <wbsecg1@gmail.com>
# Shanghai, China.
#
# ############################## HOW TO ##################################
# Suppose the library name is XX
# Usually what you need to change are: LIB_VERSION, NAME and DLLDESTDIR.
# And rename xx-buildlib and LIBXX_PRI_INCLUDED
# the contents of libXX.pro is:
#    TEMPLATE = lib
#    QT -= gui
#    CONFIG *= xx-buildlib
#    STATICLINK = 1 #optional. default is detected by staticlib in CONFIG
#    PROJECTROOT = $$PWD/..
#    include(libXX.pri)
#    preparePaths($$OUT_PWD/../out)
#    HEADERS = ...
#    SOURCES = ...
#    ...
# the content of other pro using this library is:
#    TEMPLATE = app
#    PROJECTROOT = $$PWD/..
#    STATICLINK = 1 #optional. default is detected by staticlib in CONFIG
#    include(dir_of_XX/libXX.pri)
#    preparePaths($$OUT_PWD/../out)
#    HEADERS = ...
#    SOURCES = ...
#

NAME = QDeviceWatcher
#STATICLINK = 1
!isEmpty(LIB$$upper($$NAME)_PRI_INCLUDED): {
        error("lib$${NAME}.pri already included")
        unset(NAME)
}
eval(LIB$$upper($$NAME)_PRI_INCLUDED = 1)

LIB_VERSION = 2.1.0 #0.x.y may be wrong for dll

# If user haven't supplied STATICLINK, then auto-detect
isEmpty(STATICLINK) {
  contains(CONFIG, staticlib) {
    STATICLINK = 1
  } else {
    STATICLINK = 0
  }
  # Override for ios. Dynamic link is only supported
  # in iOS 8.1.
  ios:STATICLINK = 1
}

TEMPLATE += fakelib
PROJECT_TARGETNAME = $$qtLibraryTarget($$NAME)
TEMPLATE -= fakelib

isEmpty(PROJECTROOT): PROJECTROOT = $$PWD/..
include($${PROJECTROOT}/common.pri)
preparePaths($$OUT_PWD/../out)
CONFIG += depend_includepath #?
mac_framework: PROJECT_TARGETNAME = $$NAME

PROJECT_SRCPATH = $$PWD
PROJECT_LIBDIR = $$qtLongName($$BUILD_DIR/lib)
INCLUDEPATH *= $$PROJECT_SRCPATH $$PROJECT_SRCPATH/.. $$PROJECT_SRCPATH/$$NAME
DEPENDPATH *= $$PROJECT_SRCPATH
#QMAKE_LFLAGS_RPATH += #will append to rpath dir

#eval() ?
!contains(CONFIG, $$lower($$NAME)-buildlib) {
    #The following may not need to change
    CONFIG *= link_prl
    mac_framework {
      LIBS += -F$$PROJECT_LIBDIR -framework $$PROJECT_TARGETNAME
    } else {
      LIBS *= -L$$PROJECT_LIBDIR -l$$qtLibName($$NAME)
      isEqual(STATICLINK, 1) {
        PRE_TARGETDEPS += $$PROJECT_LIBDIR/$$qtStaticLib($$NAME)
      } else {
        win32 {
          PRE_TARGETDEPS *= $$PROJECT_LIBDIR/$$qtSharedLib($$NAME, $$LIB_VERSION)
        } else {
            PRE_TARGETDEPS *= $$PROJECT_LIBDIR/$$qtSharedLib($$NAME)
        }
      }
    }
} else {
        #Add your additional configuration first. e.g.

#	win32: LIBS += -lUser32
# The following may not need to change
    !CONFIG(plugin) {
        #TEMPLATE = lib
        VERSION = $$LIB_VERSION
        DESTDIR= $$PROJECT_LIBDIR
    }
        TARGET = $$PROJECT_TARGETNAME ##I commented out this before, why?
        CONFIG *= create_prl #
        isEqual(STATICLINK, 1) {
                CONFIG -= shared dll ##otherwise the following shared is true, why?
                CONFIG *= staticlib
        } else {
                DEFINES += BUILD_$$upper($$NAME)_LIB #win32-msvc*
                CONFIG *= shared #shared includes dll
        }

        shared {
        !CONFIG(plugin) {
            !isEqual(DESTDIR, $$BUILD_DIR/bin): DLLDESTDIR = $$BUILD_DIR/bin #copy shared lib there
        }
                CONFIG(release, debug|release): !isEmpty(QMAKE_STRIP): QMAKE_POST_LINK = -$$QMAKE_STRIP $$PROJECT_LIBDIR/$$qtSharedLib($$NAME)
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
!no_rpath:!cross_compile:set_rpath($$PROJECT_LIBDIR)

unset(LIB_VERSION)
unset(PROJECT_SRCPATH)
unset(PROJECT_LIBDIR)
unset(PROJECT_TARGETNAME)

