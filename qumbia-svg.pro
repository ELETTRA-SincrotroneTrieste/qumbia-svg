#-------------------------------------------------
#
# Project created by QtCreator 2019-05-09T16:46:14
#
#-------------------------------------------------

include(qumbia-svg.pri)

CONFIG += debug

VERSION_HEX = 0x01000
VERSION = 1.0.0

TARGET = $${qumbia_svg_LIB}
TEMPLATE = lib

DEFINES -= QT_NO_DEBUG_OUTPUT

DEFINES += QUMBIASVG_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES +=

HEADERS +=

DISTFILES += \
    qumbia-svg.pri \
    Doxyfile

unix {
    doc.commands = \
    doxygen \
    Doxyfile;

    doc.files = doc/*
    doc.path = $${QUMBIA_SVG_DOCDIR}
    QMAKE_EXTRA_TARGETS += doc

    inc.files = $${HEADERS}
    inc.path = $${QUMBIA_SVG_INCLUDES}

    other_inst.files = $${DISTFILES}
    other_inst.path = $${QUMBIA_SVG_INCLUDES}

    target.path = $${QUMBIA_SVG_LIBDIR}
    INSTALLS += target inc other_inst

#message("=====================================")
#message("DOC INSTALL SKIPPED!!!!!!!!!!!!!!!!")
#message("=====================================")

    !android-g++ {
        INSTALLS += doc
    }
} # unix

# generate pkg config file
CONFIG += create_pc create_prl no_install_prl

    QMAKE_PKGCONFIG_NAME = $${qumbia_svg_LIB}
    QMAKE_PKGCONFIG_DESCRIPTION = cumbia module for Qt SVG integration
    QMAKE_PKGCONFIG_PREFIX = $${INSTALL_ROOT}
    QMAKE_PKGCONFIG_LIBDIR = $${target.path}
    QMAKE_PKGCONFIG_INCDIR = $${inc.path}
    QMAKE_PKGCONFIG_VERSION = $${VERSION}
    QMAKE_PKGCONFIG_DESTDIR = pkgconfig

INCLUDEPATH += src

# remove ourselves from -l (.pri)
LIBS -= -l$${qumbia_svg_LIB}
