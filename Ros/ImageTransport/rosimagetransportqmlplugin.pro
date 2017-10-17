TEMPLATE = lib
TARGET = rosimagetransportqmlplugin
uri      = Ros.ImageTransport

CONFIG += qt plugin c++11 nostrip link_pkgconfig
PKGCONFIG += roscpp topic_tools roslib

QT += qml quick

SOURCES += \
    src/Subscriber.cpp \
    src/Image.cpp \
    src/ImageView.cpp

HEADERS += \
    src/Subscriber.h \
    src/Image.h \
    src/ImageView.h

OTHER_FILES += qmldir

unix {
    isEmpty(PREFIX) {
      installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    }
    else {
      installPath = $$PREFIX/lib/qt5/qml/$$replace(uri, \\., /)
    }

    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}

