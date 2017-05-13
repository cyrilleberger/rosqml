#include "RosObject.h"

#include <ros/publisher.h>

class MessageDefinition;

class Publisher : public RosObject
{
  Q_OBJECT
  Q_PROPERTY(QString dataType READ dataType WRITE setDataType NOTIFY dataTypeChanged)
  Q_PROPERTY(QString topicName READ topicName WRITE setTopicName NOTIFY topicNameChanged)
  Q_PROPERTY(int queueSize READ queueSize WRITE setQueueSize NOTIFY queueSizeChanged)
  Q_PROPERTY(bool latch READ isLatch WRITE setLatch NOTIFY latchChanged)
  Q_PROPERTY(MessageDefinition* messageDefinition READ messageDefinition NOTIFY messageDefinitionChanged)
public:
  Publisher(QObject* _parent = nullptr);
  ~Publisher();
  QString topicName() const { return m_topic_name; }
  void setTopicName(const QString& _topicName);
  QString dataType() const { return m_data_type; }
  void setDataType(const QString& _topicName);
  int queueSize() const { return m_queue_size; }
  void setQueueSize(int _qS);
  bool isLatch() const { return m_latch; }
  void setLatch(bool _l);
  Q_INVOKABLE void publish(const QVariant& _message);
  MessageDefinition* messageDefinition() const { return m_message_definition; }
signals:
  void dataTypeChanged();
  void topicNameChanged();
  void queueSizeChanged();
  void latchChanged();
  void messageDefinitionChanged();
private:
  void start_publisher();
  QString m_topic_name, m_data_type;
  int m_queue_size = 1;
  bool m_latch;
  ros::Publisher m_publisher;
  MessageDefinition* m_message_definition = nullptr;
};
