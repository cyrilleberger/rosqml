#include "MessageMessageField.h"

#include <QVariant>

#include <ros/serialization.h>

#include "MessageDefinition.h"

MessageMessageField::~MessageMessageField()
{
}

QVariant MessageMessageField::parse(ros::serialization::IStream& _stream) const
{
  if(count() == 1)
  {
    return m_md->parse(_stream);
  } else {
    QVariantList l;
    for(int i = 0; i < count(); ++i)
    {
      l.append(m_md->parse(_stream));
    }
    return l;
  }
}
void MessageMessageField::generate(ros::serialization::OStream & _stream, const QVariant & _variant) const
{
  if(count() == 1)
  {
    m_md->generate(_variant.toMap(), _stream);
  } else {
    QVariantList l = _variant.toList();
    int i = 0;
    for(; i < std::min(l.size(), count()); ++i)
    {
      m_md->generate(l[i].toMap(), _stream);
    }
    for(; i < count(); ++i)
    {
      m_md->generate(QVariantMap(), _stream);
    }
  }
}
void MessageMessageField::serializedLength(ros::serialization::LStream& _stream, const QVariant & _variant) const
{
  if(count() == 1)
  {
    m_md->serializedLength(_variant.toMap(), _stream);
  } else {
    QVariantList l = _variant.toList();
    int i = 0;
    for(; i < std::min(l.size(), count()); ++i)
    {
      m_md->serializedLength(l[i].toMap(), _stream);
    }
    for(; i < count(); ++i)
    {
      m_md->serializedLength(QVariantMap(), _stream);
    }
  }
}
