isEmpty(CUMBIA_ROOT) {
    CUMBIA_ROOT=/usr/local/cumbia-libs
}
include($${CUMBIA_ROOT}/include/quapps/quapps.pri)

CONFIG += debug

CONFIG += link_pkgconfig
PKGCONFIG += qgraphicsplot

TEMPLATE = lib
libname=qusvg-extensions

QT += xml opengl

# change the place where you want the include files to be installed
INC_DIR = $${INSTALL_ROOT}/include/qusvg-extensions

# change to your desired lib dir
LIB_DIR = $${INSTALL_ROOT}/lib
DEFINES -= QT_NO_DEBUG_OUTPUT

VERSION_HEX = 0x010000
VERSION = 1.0.0
VER_MAJ = 1
VER_MIN = 0
VER_FIX = 0

DEFINES += QUSVG_EXTENSIONS_VERSION_STR=\"\\\"$${VERSION}\\\"\" \
    QUSVG_EXTENSIONS_VERSION=$${VERSION_HEX} \
    VER_MAJ=$${VER_MAJ} \
    VER_MIN=$${VER_MIN} \
    VER_FIX=$${VER_FIX}

# append a "-qt6" suffix to the library built with qt5
greaterThan(QT_MAJOR_VERSION, 5) {
    QTVER_SUFFIX = -qt$${QT_MAJOR_VERSION}
} else {
    QTVER_SUFFIX =
}

TARGET = $${libname}$${QTVER_SUFFIX}

packagesExist(qumbia-svg) {
  PKGCONFIG += qumbia-svg
    message("qusvgexample: found module qumbia-svg ")
} else {
    error("qusvgexample: module qumbia-svg is required - check PKG_CONFIG_PATH")
}

packagesExist(qgraphicsplot) {
  PKGCONFIG += qgraphicsplot
    message("qusvgexample: found module qgraphicsplot ")
} else {
    error("qusvgexample: module qgraphicsplot is required - check PKG_CONFIG_PATH")
}

# RESOURCES +=

SOURCES += src/main.cpp \
                src/qusvg_extensions.cpp \
                src/qusvgitemplot.cpp

PUBLICHEADERS += src/qusvg_extensions.h \
    src/qusvgitemplot.h

# cuuimake runs uic
# FORMS    = src/qusvg_extensions.ui
# but we need to include ui_xxxx.h file amongst the headers
# in order to be recompiled when it changes
#

HEADERS += $${PUBLICHEADERS}
    ui/ui_qusvg_extensions.h

# - ui: where to find cuuimake ui_*.h files
#   since FORMS is not used
# - src: where to find headers included by
#   ui_*.h (e.g. for custom widget promoted
#   from the Qt designer)
#
INCLUDEPATH += ui src

TARGET = qusvg-extensions

inc.files = $${PUBLICHEADERS}
inc.path = $${INC_DIR}

target.path=$${LIB_DIR}


doc.commands = \
    doxygen \
    Doxyfile;

doc.files = doc/
doc.path = $${DOC_DIR}

#
# make install works if INSTALL_DIR is given to qmake
#
!isEmpty(INSTALL_DIR) {
    wasm-emscripten {
        inst.files = wasm/*
    } else {
        inst.files = $${TARGET}
    }
    inst.path = $${INSTALL_DIR}
    INSTALLS += inst
    message("-")
    message("INSTALLATION")
    message("       execute `make install` to install 'qusvg_extensions' under $${INSTALL_DIR} ")
    message("-")
} else {
    message("-")
    message("INSTALLATION")
    message("       call qmake INSTALL_DIR=/usr/local/bin to install qusvg_extensions later with `make install` ")
    message("-")
}

# unix:LIBS += -L. -lmylib

# unix:INCLUDEPATH +=  . ../../src

# generate pkg config file
CONFIG += create_pc create_prl no_install_prl

message("libname $${libname}")
message("prefix $${INSTALL_ROOT}")
message("target $${target.path}")
message("inc $${inc.path} ")
message("version $${VERSION} ")

QMAKE_PKGCONFIG_NAME = $${libname}
QMAKE_PKGCONFIG_DESCRIPTION = "extensions to the qumbia-svg library, offering items"
QMAKE_PKGCONFIG_PREFIX = $${INSTALL_ROOT}
QMAKE_PKGCONFIG_LIBDIR = $${target.path}
QMAKE_PKGCONFIG_INCDIR = $${inc.path}
QMAKE_PKGCONFIG_VERSION = $${VERSION}
QMAKE_PKGCONFIG_DESTDIR = pkgconfig

pkgconfig_f.path = $${LIB_DIR}/pkgconfig
pkgconfig_f.files = pkgconfig/$${libname}.pc

# INSTALLS = inc lib doc data

INSTALLS = inc target pkgconfig_f


RESOURCES += \
    src/qusvg_extensions.qrc
