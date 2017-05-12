#include "RosObject.h"

class Publisher : public RosObject
{
public:
  Publisher(QObject* _parent = nullptr);
  ~Publisher();
};
