#ifndef ROSWRAPPER_H_
#define ROSWRAPPER_H_

#include "RosObject.h"

class RosWrapper : public RosObject
{
  Q_PROPERTY(quint64 startTime READ startTime CONSTANT)
public:
  RosWrapper(QObject* _parent = nullptr);
  ~RosWrapper();
  quint64 startTime() const;
protected:
};

#endif

