TEMPLATE = lib
TARGET = rosimagetransportqmlplugin
uri      = Ros.ImageTransport

CONFIG += qt plugin c++11 nostrip link_pkgconfig
PKGCONFIG += roscpp image_transport roslib

QT += qml quick concurrent

SOURCES += \
    src/RosImageTransportQmlPlugin.cpp \
    src/Subscriber.cpp \
    src/Image.cpp \
    src/ImageViewItem.cpp

HEADERS += \
    src/RosImageTransportQmlPlugin.h \
    src/Subscriber.h \
    src/Image.h \
    src/ImageViewItem.h

OTHER_FILES += qmldir

qmldir.files = qmldir

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

