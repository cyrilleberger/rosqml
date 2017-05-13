#include "MessageMessageField.h"

#include <QVariant>

#include <ros/serialization.h>

#include "MessageDefinition.h"

MessageMessageField::~MessageMessageField()
{
}

QVariant MessageMessageField::parse(ros::serialization::IStream& _stream) const
{
  return m_md->parse(_stream);
}
void MessageMessageField::generate(ros::serialization::OStream & _stream, const QVariant & _variant) const
{
  m_md->generate(_variant.toMap(), _stream);
}
void MessageMessageField::serializedLength(ros::serialization::LStream& _stream, const QVariant & _variant) const
{
  m_md->serializedLength(_variant.toMap(), _stream);
}

