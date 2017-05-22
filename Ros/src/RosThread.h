#ifndef ROSTHREAD_H
#define ROSTHREAD_H

#include <QThread>

class RosThread : public QThread
{
  RosThread();
public:
  static RosThread* instance();
  quint64 now();
protected:
  void run();
};

#endif // ROSTHREAD_H
