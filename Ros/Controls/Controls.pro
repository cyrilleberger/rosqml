TEMPLATE=lib

uri      = Ros/Controls

OTHER_FILES += qmldir

qmldir.files = qmldir MessageView.qml Event.qml Events.qml EventsLogger.qml

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
