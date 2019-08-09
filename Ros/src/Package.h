#pragma once

#include <QObject>

class Package : public QObject
{
  Q_OBJECT
public:
  Package(QObject* _parent);
  ~Package();
  Q_INVOKABLE QString getPath(const QString& _path);
};
