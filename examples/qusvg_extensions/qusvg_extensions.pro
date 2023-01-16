isEmpty(CUMBIA_ROOT) {
    CUMBIA_ROOT=/usr/local/cumbia-libs
}
include($${CUMBIA_ROOT}/include/quapps/quapps.pri)

CONFIG += debug

CONFIG += link_pkgconfig
PKGCONFIG += qgraphicsplot

QT += xml

DEFINES -= QT_NO_DEBUG_OUTPUT

packagesExist(qumbia-svg) {
  PKGCONFIG += qumbia-svg
    message("qusvgexample: found module qumbia-svg ")
} else {
    error("qusvgexample: module qumbia-svg is required - check PKG_CONFIG_PATH")
}

# RESOURCES +=

SOURCES += src/main.cpp \
                src/qusvg_extensions.cpp \
                src/qusvgitemplot.cpp

HEADERS += src/qusvg_extensions.h \
    src/qusvgitemplot.h

# cuuimake runs uic
# FORMS    = src/qusvg_extensions.ui
# but we need to include ui_xxxx.h file amongst the headers
# in order to be recompiled when it changes
#
HEADERS += \
    ui/ui_qusvg_extensions.h

# - ui: where to find cuuimake ui_*.h files
#   since FORMS is not used
# - src: where to find headers included by
#   ui_*.h (e.g. for custom widget promoted
#   from the Qt designer)
#
INCLUDEPATH += ui src

TARGET = qusvg_extensions

!wasm-emscripten {
    TARGET   = bin/$${TARGET}
} else {
    TARGET = wasm/$${TARGET}
}

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

message("-")
message("NOTE")
message("You need to run cuuimake in order to build the project")
message("-")
message("        cuuimake --show-config to see cuuimake configuration options")
message("        cuuimake --configure to configure cuuimake")
message("        cuuimake -jN to execute cuuimake and then make -jN")
message("        cuuimake --make to run cuuimake and then make")
message("-")

RESOURCES += \
    src/qusvg_extensions.qrc
