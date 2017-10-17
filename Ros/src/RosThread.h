#ifndef ROSTHREAD_H
#define ROSTHREAD_H

#include <QThread>

class RosThread : public QThread
{
  RosThread();
public:
  static RosThread* instance();
  quint64 now() const;
  quint64 startTime() const { return m_startTime; }
protected:
  void run();
private:
  quint64 m_startTime;
};

#endif // ROSTHREAD_H
