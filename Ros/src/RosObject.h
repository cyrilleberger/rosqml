#ifndef ROSOBJECT_H_
#define ROSOBJECT_H_

#include <QObject>

#include <ros/node_handle.h>

class RosObject : public QObject
{
public:
  RosObject(QObject* _parent = nullptr);
  ~RosObject();
protected:
  ros::NodeHandle m_handle;
};

#endif
