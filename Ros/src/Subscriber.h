#include "RosObject.h"

#include <ros/message_event.h>
#include <topic_tools/shape_shifter.h>


class Subscriber : public RosObject
{
  Q_OBJECT
  Q_PROPERTY(QString topicName READ topicName WRITE setTopicName NOTIFY topicNameChanged)
  Q_PROPERTY(int queueSize READ queueSize WRITE setQueueSize NOTIFY queueSizeChanged)
public:
  Subscriber(QObject* _parent = nullptr);
  ~Subscriber();
  QString topicName() const { return m_topic_name; }
  void setTopicName(const QString& _topicName);
  int queueSize() const { return m_queue_size; }
  void setQueueSize(int _qS);
signals:
  void topicNameChanged();
  void queueSizeChanged();
private:
  void callback(ros::MessageEvent<const topic_tools::ShapeShifter> _message);
  void subscribe();
  QString m_topic_name;
  int m_queue_size;
  ros::Subscriber m_subscriber;
};
