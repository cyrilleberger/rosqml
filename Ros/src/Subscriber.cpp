#include "Subscriber.h"

#include <QDebug>

#include <topic_tools/shape_shifter.h>

#include "MessageDefinition.h"

Subscriber::Subscriber(QObject* _parent) : RosObject(_parent), m_queue_size(1), m_skip(0), m_skipCount(0)
{
}

Subscriber::~Subscriber()
{
}

void Subscriber::setTopicName(const QString& _topicName)
{
  m_topic_name = _topicName;
  emit(topicNameChanged());
  subscribe();
}

void Subscriber::setQueueSize(int _qS)
{
  m_queue_size = _qS;
  emit(queueSizeChanged());
  subscribe();
}

void Subscriber::callback(ros::MessageEvent<const topic_tools::ShapeShifter> _message)
{
  if(m_skipCount < m_skip)
  {
    ++m_skipCount;
    return;
  }
  m_skipCount = 0;
  MessageDefinition* md = MessageDefinition::get(QString::fromStdString(_message.getMessage()->getDataType()));
  if(md != m_message_definition)
  {
    m_message_definition = md;
    emit(messageDefinitionChanged());
  }
  
  QByteArray arr;
  arr.resize(_message.getMessage()->size());
  
  ros::serialization::OStream stream(reinterpret_cast<uint8_t*>(arr.data()), arr.size());
  _message.getMessage()->write(stream);
  
  QVariantMap h = md->deserializeMessage(arr);
  m_lastMessage = h;
  emit(messageReceived(h, _message.getReceiptTime().toNSec(), QString::fromStdString(_message.getPublisherName())));
}

void Subscriber::subscribe()
{
  if(m_topic_name.isEmpty() or m_queue_size <= 0)
  {
    m_subscriber = ros::Subscriber();
  } else {
    m_subscriber = m_handle.subscribe<topic_tools::ShapeShifter>(m_topic_name.toStdString().c_str(), m_queue_size,
                                                                boost::bind(&Subscriber::callback, this, _1));
  }
}

