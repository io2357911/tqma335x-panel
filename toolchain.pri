
contains(PLATFORM, arm) {
    TOOLCHAIN_PREFFIX=arm-buildroot-linux-gnueabihf
    TOOLCHAIN_BINUTILS_PATH = /home/nullptr/proj/misc/panel-tqma3352/buildroot-2017.02.7/output/host/usr/bin
    TOOLCHAIN_SYSROOT_PATH = /home/nullptr/proj/misc/panel-tqma3352/buildroot-2017.02.7/output/host/usr/arm-buildroot-linux-gnueabihf/sysroot

    COMPILE_FLAGS = "-march=armv7-a -marm -mfpu=vfpv3  -mfloat-abi=hard --sysroot=$$TOOLCHAIN_SYSROOT_PATH"
#    COMPILE_FLAGS = "-march=armv7-a -mfpu=neon  -mfloat-abi=hard -mcpu=cortex-a8 --sysroot=$$TOOLCHAIN_SYSROOT_PATH"

    QMAKE_CC = "$$TOOLCHAIN_BINUTILS_PATH/$$TOOLCHAIN_PREFFIX-gcc $$COMPILE_FLAGS"
    QMAKE_CXX = "$$TOOLCHAIN_BINUTILS_PATH/$$TOOLCHAIN_PREFFIX-g++ $$COMPILE_FLAGS"
    QMAKE_LINK= $$TOOLCHAIN_BINUTILS_PATH/$$TOOLCHAIN_PREFFIX-g++
    QMAKE_LINK_SHLIB = $$TOOLCHAIN_BINUTILS_PATH/$$TOOLCHAIN_PREFFIX-g++

    QMAKE_AR = $$TOOLCHAIN_BINUTILS_PATH/$$TOOLCHAIN_PREFFIX-ar cqs
    QMAKE_OBJCOPY = $$TOOLCHAIN_BINUTILS_PATH/$$TOOLCHAIN_PREFFIX-objcopy
    QMAKE_STRIP = $$TOOLCHAIN_BINUTILS_PATH/$$TOOLCHAIN_PREFFIX-strip

    QMAKE_INCDIR =
    QMAKE_LFLAGS_RPATH =
    QMAKE_LFLAGS = "-Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed -Wl,--dynamic-linker=/lib/ld-linux-armhf.so.3 --sysroot=$$TOOLCHAIN_SYSROOT_PATH"

    INCLUDEPATH = \
        $$TOOLCHAIN_SYSROOT_PATH/usr/include \
        $$TOOLCHAIN_SYSROOT_PATH/usr/include/c++/4.9.4 \
        $$TOOLCHAIN_SYSROOT_PATH/usr/include/c++/4.9.4/arm-buildroot-linux-gnueabihf

    LIBS = \
        -L$$TOOLCHAIN_SYSROOT_PATH/lib \
        -L$$TOOLCHAIN_SYSROOT_PATH/usr/lib

    QMAKE_LIBS_OPENGL =
}

contains(PLATFORM, pc) {

}
