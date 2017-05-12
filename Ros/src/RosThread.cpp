#include "RosThread.h"

#include <ros/ros.h>

RosThread::RosThread()
{

}

RosThread* RosThread::instance()
{
  static RosThread* rt = nullptr;
  if(not rt)
  {
    ros::init(ros::M_string(), "rosqml", ros::init_options::AnonymousName);
    rt = new RosThread();
  }
  return rt;
}

void RosThread::run()
{
  ros::spin();
}
