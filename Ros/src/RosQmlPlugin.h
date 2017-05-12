#ifndef ROSQMLPLUGIN_H
#define ROSQMLPLUGIN_H

#include <QQmlExtensionPlugin>

class RosQmlPlugin : public QQmlExtensionPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface")
public:
  RosQmlPlugin();
  void registerTypes(const char *uri);
  void initializeEngine(QQmlEngine *engine, const char *uri);
};

#endif // ROSQMLPLUGIN_H
