#ifndef ROSIMAGETRANSPORTQMLPLUGIN_H
#define ROSIMAGETRANSPORTQMLPLUGIN_H

#include <QQmlExtensionPlugin>

class RosImageTransportQmlPlugin : public QQmlExtensionPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
  RosImageTransportQmlPlugin();
  void registerTypes(const char *uri);
};

#endif // ROSQMLPLUGIN_H

