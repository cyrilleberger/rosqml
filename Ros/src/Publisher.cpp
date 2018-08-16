#include "Publisher.h"

#include <QVariant>

#include <topic_tools/shape_shifter.h>

#include "MessageDefinition.h"

Publisher::Publisher(QObject* _parent) : RosObject(_parent)
{
}

Publisher::~Publisher()
{
}


void Publisher::setDataType(const QString& _topicName)
{
  m_data_type = _topicName;
  emit(dataTypeChanged());
  start_publisher();
}

void Publisher::setTopicName(const QString& _topicName)
{
  m_topic_name = _topicName;
  emit(topicNameChanged());
  start_publisher();
}

void Publisher::setQueueSize(int _qS)
{
  m_queue_size = _qS;
  emit(queueSizeChanged());
  start_publisher();
}

void Publisher::setLatch(bool _qS)
{
  m_latch = _qS;
  emit(latchChanged());
  start_publisher();
}

void Publisher::start_publisher()
{
  if(m_data_type.isEmpty() or m_topic_name.isEmpty() or m_queue_size == 0)
  {
    m_publisher = ros::Publisher();
  } else {
    m_message_definition = MessageDefinition::get(m_data_type);
    emit(messageDefinitionChanged());
    ros::AdvertiseOptions ao(m_topic_name.toStdString(), m_queue_size, m_message_definition->md5().toHex().toStdString(), m_data_type.toStdString(), m_message_definition->definition().toStdString());
    m_publisher = m_handle.advertise(ao);
  }
}

void Publisher::publish(const QVariant& _message)
{
  if(m_message_definition)
  {
    QVariantMap message = m_message_definition->variantToMap(_message);
    QByteArray data = m_message_definition->serializeMessage(message);
    ros::serialization::IStream stream(reinterpret_cast<uint8_t*>(data.data()), data.size());
    
    topic_tools::ShapeShifter ss;
    ss.morph(m_message_definition->md5().toHex().toStdString(), m_data_type.toStdString(), m_message_definition->definition().toStdString(), std::string());
    ss.read(stream);

    m_publisher.publish(ss);
  }
}
