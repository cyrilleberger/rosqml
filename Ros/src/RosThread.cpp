#include "RosThread.h"

#include <QCoreApplication>
#include <QDebug>
#include <QProcessEnvironment>

#include <ros/ros.h>

#include "IOFormat.h"

RosThread::RosThread() : m_ros_is_started(false)
{

}

RosThread* RosThread::instance()
{
  static RosThread* rt = nullptr;
  if(not rt)
  {
    QString ros_arguments = QProcessEnvironment::systemEnvironment().value("ROS_ARGUMENTS");
    qDebug().nospace().noquote() << IOFormat::green << "Using ros_arguments: '" << ros_arguments << "'" << IOFormat::reset;
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
    rt = new RosThread();
    if(not QProcessEnvironment::systemEnvironment().contains("ROS_DELAY_START"))
    {
      ros::start();
      rt->m_ros_is_started = true;
    }
    QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, [](){
      ros::shutdown();
    });
  }
  return rt;
}

void RosThread::run()
{
  if(not m_ros_is_started)
  {
    ros::start();
    m_ros_is_started = true;
  }
  m_startTime = now();
  ros::spin();
}

quint64 RosThread::now() const
{
  return ros::Time::now().toNSec();
}
