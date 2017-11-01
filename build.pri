#В QtCreator нужно указать CPU=x86 или CPU=x64 в зависимости от платформы, на которой собирают

#QMAKE_CXXFLAGS += -D_GLIBCXX_USE_CXX11_ABI=0

CONFIG(debug, debug|release) {
    CONFIG_NAME = Debug
} else {
    CONFIG_NAME = Release
    DEFINES += BUILD_RELEASE
}

RCC_DIR = build/$$PLATFORM/$$CONFIG_NAME
UI_DIR = build/$$PLATFORM/$$CONFIG_NAME
MOC_DIR = build/$$PLATFORM/$$CONFIG_NAME
OBJECTS_DIR = build/$$PLATFORM/$$CONFIG_NAME

DESTDIR = build/$$PLATFORM/$$CONFIG_NAME

QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

QMAKE_CXXFLAGS_RELEASE *= -Os

contains(PLATFORM, BeagleBone) {
    DEFINES += BUILD_ARM
}
