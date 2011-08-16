
#$$[TARGET_PLATFORM]
#$$[QT_ARCH] #windows symbian windowsce arm
PLATFORM_EXT =
ARCH_EXT =
TOOLCHAIN_EXT =
unix {
  PLATFORM_EXT = _unix
  *linux*: PLATFORM_EXT = _linux
  maemo5: PLATFORM_EXT = _maemo5
} else:win32 {
  PLATFORM_EXT = _win32
} else:macx {
  PLATFORM_EXT = _macx
}

#*arm*: ARCH_EXT = $${ARCH_EXT}_arm
#isEqual(QT_ARCH, arm) {
contains(QT_ARCH, arm.*) {
  ARCH_EXT = $${ARCH_EXT}_$${QT_ARCH}
}
*64:   ARCH_EXT = $${ARCH_EXT}_x64

*llvm*: TOOLCHAIN_EXT = _llvm
#*msvc*:

#moc, uic, rcc are the same in the same qt version
DESTDIR     = bin$${PLATFORM_EXT}$${ARCH_EXT}
OBJECTS_DIR = .obj/$${PLATFORM_EXT}$${ARCH_EXT}$${TOOLCHAIN_EXT}
MOC_DIR = .moc/$$[QT_VERSION]
RCC_DIR = .rcc/$$[QT_VERSION]
UI_DIR  = .ui/$$[QT_VERSION]
