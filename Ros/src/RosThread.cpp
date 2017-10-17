#include "RosThread.h"

#include <QDebug>
#include <QProcessEnvironment>

#include <ros/ros.h>

RosThread::RosThread()
{

}

RosThread* RosThread::instance()
{
  static RosThread* rt = nullptr;
  if(not rt)
  {
    QString ros_arguments = QProcessEnvironment::systemEnvironment().value("ROS_ARGUMENTS");
    ros::M_string map;
    for(QString arg : ros_arguments.split(' '))
    {
      QStringList split = arg.split(":=");
      if(split.size() == 2)
      {
        map[split[0].toStdString()] = split[1].toStdString();
      } else {
        qWarning() << "Unhandled ros argument: " << arg;
      }
    }
    ros::init(map, "rosqml", ros::init_options::AnonymousName);
    ros::start();
    rt = new RosThread();
  }
  return rt;
}

void RosThread::run()
{
  m_startTime = now();
  ros::spin();
}

quint64 RosThread::now() const
{
  return ros::Time::now().toNSec();
}
