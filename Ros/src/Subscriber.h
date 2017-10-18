#include "RosObject.h"

#include <QVariantMap>

#include <ros/message_event.h>
#include <topic_tools/shape_shifter.h>

class MessageDefinition;

class Subscriber : public RosObject
{
  Q_OBJECT
  Q_PROPERTY(QString topicName READ topicName WRITE setTopicName NOTIFY topicNameChanged)
  Q_PROPERTY(int queueSize READ queueSize WRITE setQueueSize NOTIFY queueSizeChanged)
  Q_PROPERTY(int skip READ skip WRITE setSkip NOTIFY skipChanged)
  Q_PROPERTY(QVariant lastMessage READ lastMessage NOTIFY messageReceived);
  Q_PROPERTY(MessageDefinition* messageDefinition READ messageDefinition NOTIFY messageDefinitionChanged)
public:
  Subscriber(QObject* _parent = nullptr);
  ~Subscriber();
  QVariant lastMessage() const { return m_lastMessage; }
  QString topicName() const { return m_topic_name; }
  void setTopicName(const QString& _topicName);
  int queueSize() const { return m_queue_size; }
  int skip() const { return m_skip; }
  void setSkip(int skip) { m_skip = skip; m_skipCount = m_skip; emit(skipChanged()); }
  void setQueueSize(int _qS);
  MessageDefinition* messageDefinition() const { return m_message_definition; }
signals:
  void topicNameChanged();
  void queueSizeChanged();
  void skipChanged();
  void messageReceived(const QVariant& message, quint64 timestamp, const QString& publisher);
  void messageDefinitionChanged();
private:
  void callback(ros::MessageEvent<const topic_tools::ShapeShifter> _message);
  void subscribe();
  QString m_topic_name;
  int m_queue_size;
  int m_skip, m_skipCount;
  ros::Subscriber m_subscriber;
  QVariantMap m_lastMessage;
  MessageDefinition* m_message_definition = nullptr;
};
