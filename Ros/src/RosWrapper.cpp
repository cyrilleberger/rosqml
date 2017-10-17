#include "RosWrapper.h"

#include "RosThread.h"

RosWrapper::RosWrapper(QObject* _parent) : RosObject(_parent)
{
}

RosWrapper::~RosWrapper()
{
}

quint64 RosWrapper::startTime() const
{
  return RosThread::instance()->startTime();
}
