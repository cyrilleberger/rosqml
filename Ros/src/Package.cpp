#include "Package.h"

#include <ros/package.h>

Package::Package(QObject* _parent) : QObject(_parent)
{
}

Package::~Package()
{
}

QString Package::getPath(const QString& _path)
{
  return QString::fromStdString(ros::package::getPath(_path.toStdString()));
}

#include "moc_Package.cpp"
