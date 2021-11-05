TARGET = qnfcdc
TEMPLATE = lib
CONFIG += create_pc create_prl no_install_prl link_pkgconfig
PKGCONFIG += libgnfcdc libglibutil
QT -= gui

include(version.pri)

QMAKE_CXXFLAGS += -Wno-unused-parameter

DEFINES += QNFCDC_LIBRARY
INCLUDEPATH += include

PKGCONFIG_NAME = $${TARGET}

isEmpty(PREFIX) {
    PREFIX=/usr
}

CONFIG(debug, debug|release) {
    DEFINES += DEBUG HARBOUR_DEBUG
}

OTHER_FILES += \
    $${PKGCONFIG_NAME}.prf \
    rpm/libqnfcdc.spec \
    LICENSE \
    README

SOURCES += \
    src/NfcAdapter.cpp \
    src/NfcMode.cpp \
    src/NfcPeer.cpp \
    src/NfcSystem.cpp \
    src/NfcTag.cpp

PUBLIC_HEADERS += \
    include/NfcAdapter.h \
    include/NfcMode.h \
    include/NfcPeer.h \
    include/NfcSystem.h \
    include/NfcTag.h

HEADERS += \
    src/Debug.h \
    $${PUBLIC_HEADERS}

target.path = $$[QT_INSTALL_LIBS]

headers.files = $${PUBLIC_HEADERS}
headers.path = $${INSTALL_ROOT}$${PREFIX}/include/$${TARGET}

pkgconfig.files = $${PKGCONFIG_NAME}.pc
pkgconfig.path = $$[QT_INSTALL_LIBS]/pkgconfig

QMAKE_PKGCONFIG_NAME = $${PKGCONFIG_NAME}
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_INCDIR = $$headers.path
QMAKE_PKGCONFIG_DESCRIPTION = D-Bus client for nfcd
QMAKE_PKGCONFIG_PREFIX = $${PREFIX}
QMAKE_PKGCONFIG_VERSION = $${VERSION}

INSTALLS += target headers pkgconfig
