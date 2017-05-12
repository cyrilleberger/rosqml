#include "Subscriber.h"

#include <QDebug>

#include <topic_tools/shape_shifter.h>

#include "MessageDefinition.h"

Subscriber::Subscriber(QObject* _parent) : RosObject(_parent), m_queue_size(1)
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
  qDebug() << _message.getMessage()->getDataType().c_str() << _message.getMessage()->getMessageDefinition().c_str() << _message.getReceiptTime ().toSec() << _message.getPublisherName ().c_str();
  
  
  MessageDefinition* md = MessageDefinition::get(QString::fromStdString(_message.getMessage()->getDataType()));
  
  qDebug() << md;
}

void Subscriber::subscribe()
{
  if(m_topic_name.isEmpty() or m_queue_size <= 0)
  {
    m_subscriber = ros::Subscriber();
  } else {
    m_subscriber = m_handle.subscribe<topic_tools::ShapeShifter>(m_topic_name.toStdString().c_str(), m_queue_size,
                                                                boost::bind(&Subscriber::callback, this, _1));
//     m_subscriber = m_handle.subscribe(m_topic_name.toStdString().c_str(), m_queue_size,
//                                                                 &Subscriber::callback, this);
  }
}

