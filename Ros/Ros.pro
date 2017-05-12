TEMPLATE = lib
TARGET = rosqmlplugin
uri      = Ros

CONFIG += qt plugin c++11 nostrip link_pkgconfig
PKGCONFIG += roscpp topic_tools roslib

QT += qml quick

SOURCES += \
    src/MessageDefinition.cpp \
    src/Subscriber.cpp \
    src/Publisher.cpp \
    src/RosObject.cpp \
    src/RosThread.cpp \
    src/RosQmlPlugin.cpp

HEADERS += \
    src/MessageDefinition.h \
    src/Subscriber.h \
    src/Publisher.h \
    src/RosThread.h \
    src/RosQmlPlugin.h

OTHER_FILES += qmldir

qmldir.files = qmldir

unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}
