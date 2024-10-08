# + ----------------------------------------------------------------- +
#
# Customization section:
#
# Customize the following paths according to your installation:
#
#
# Here qumbia-svg will be installed
# INSTALL_ROOT can be specified from the command line running qmake "INSTALL_ROOT=/my/install/path"
#

isEmpty(INSTALL_ROOT) {
    INSTALL_ROOT = /usr/local/cumbia-libs
}

#
#
# Here qumbia-svg include files will be installed
    QUMBIA_SVG_INCLUDES=$${INSTALL_ROOT}/include/qumbia-svg
#
#
# Here qumbia-svg share files will be installed
#
    QUMBIA_SVG_SHARE=$${INSTALL_ROOT}/share/qumbia-svg
#
#
# Here qumbia-svg libraries will be installed
    QUMBIA_SVG_LIBDIR=$${INSTALL_ROOT}/lib
#
#
# Here qumbia-svg documentation will be installed
    QUMBIA_SVG_DOCDIR=$${INSTALL_ROOT}/share/doc/qumbia-svg
#
# The name of the library
    qumbia_svg_LIB=qumbia-svg
#
#

VERSION_HEX = 0x020201
VERSION = 2.2.1

DEFINES += QUMBIA_SVG_VERSION_STR=\"\\\"$${VERSION}\\\"\" \
    QUMBIA_SVG_VERSION=$${VERSION_HEX}

MOC_DIR=moc
OBJECTS_DIR=objs

unix:!android-g++ {
    CONFIG += link_pkgconfig
    PKGCONFIG += cumbia cumbia-qtcontrols
}

QT       += widgets xml svg opengl

greaterThan(QT_MAJOR_VERSION, 5) {
    QT += svgwidgets openglwidgets
}

LIBS += -L$${INSTALL_ROOT}/lib -l$${qumbia_svg_LIB}

