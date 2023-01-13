#-------------------------------------------------
#
# Project created by QtCreator 2019-05-09T16:46:14
#
#-------------------------------------------------

include(qumbia-svg.pri)

CONFIG += debug

VERSION_HEX = 0x0100100
VERSION = 1.1.0

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

SOURCES += \
        src/cumbia/qusvgconnectionspool.cpp \
        src/cumbia/qusvgresultdata.cpp \
        src/cumbia/qusvgresultdatainterpreter.cpp \
        src/qugraphicssvgitem.cpp \
        src/qugraphicssvgitemxtensionfactory.cpp \
        src/qusvglayerhelper.cpp \
        src/cumbia/qusvgreader.cpp \
        src/qudom.cpp \
        src/qudomelement.cpp \
        src/qusvg.cpp \
        src/qusvgreadlink.cpp \
        src/qusvgview.cpp \
        src/events/qusvgwriteactionprovider.cpp \
        src/events/qusvgitemeventhandler.cpp \
        src/events/qusvghelperappactionprovider.cpp

HEADERS += \
    src/cumbia/qusvgconnectionspool.h \
    src/cumbia/qusvgdatalistener.h \
    src/cumbia/qusvgreplacewildcardhelperinterface.h \
    src/cumbia/qusvgresultdata.h \
    src/cumbia/qusvgresultdatainterpreter.h \
    src/qugraphicssvgitem.h \
    src/qugraphicssvgitemxtensionfactory.h \
    src/qusvglayerhelper.h \
        src/cumbia/qusvgreader.h \
        src/qudom.h \
        src/qudomelement.h \
        src/qusvg.h \
    src/qusvgreadlink.h \
        src/qusvgview.h \
    src/events/qusvghelperappactionprovider.h \
    src/events/qusvgitemeventhandler.h \
    src/events/qusvgwriteactionprovider.h

DISTFILES += \
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

INCLUDEPATH += src src/cumbia src/events

# remove ourselves from -l (.pri)
LIBS -= -l$${qumbia_svg_LIB}

# RESOURCES += \
#    qumbia-svg.qrc
