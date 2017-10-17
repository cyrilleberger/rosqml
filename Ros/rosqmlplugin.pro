TEMPLATE = lib
TARGET = rosqmlplugin
uri      = Ros

CONFIG += qt plugin c++11 nostrip link_pkgconfig
PKGCONFIG += roscpp topic_tools roslib

QT += qml quick

SOURCES += \
    src/MessageDefinition.cpp \
    src/MessageField.cpp \
    src/MessageMessageField.cpp \
    src/Subscriber.cpp \
    src/Publisher.cpp \
    src/RosObject.cpp \
    src/RosThread.cpp \
    src/RosQmlPlugin.cpp \
    src/RosWrapper.cpp

HEADERS += \
    src/MessageDefinition.h \
    src/MessageField.h \
    src/MessageMessageField.h \
    src/Subscriber.h \
    src/Publisher.h \
    src/RosThread.h \
    src/RosQmlPlugin.h \
    src/RosWrapper.h

OTHER_FILES += qmldir

qmldir.files = qmldir qml/Time.js

unix {
    isEmpty(PREFIX) {
      installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    }
    else {
      installPath = $$PREFIX/lib/qt5/qml/$$uri
    }

    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}
