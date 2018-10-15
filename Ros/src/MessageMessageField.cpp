#include "MessageMessageField.h"

#include <QVariant>

#include <ros/serialization.h>

#include "MessageDefinition.h"

MessageMessageField::~MessageMessageField()
{
}

QVariant MessageMessageField::deserialize(ros::serialization::IStream& _stream) const
{
  switch(count())
  {
    case 1:
      return m_md->deserializeMessage(_stream);
    case -1:
    {
      QVariantList l;
      qint32 size;
      _stream.next(size);
      for(int i = 0; i < size; ++i)
      {
        l.append(m_md->deserializeMessage(_stream));
      }
      return l;
    }
    default:
    {
      QVariantList l;
      for(int i = 0; i < count(); ++i)
      {
        l.append(m_md->deserializeMessage(_stream));
      }
      return l;
    }
  }
}
void MessageMessageField::serialize(ros::serialization::OStream & _stream, const QVariant & _variant) const
{
  switch(count())
  {
    case 1:
      m_md->serializeMessage(_variant.toMap(), _stream);
      break;
    case -1:
    {
      QVariantList l = _variant.toList();
      _stream.next(qint32(l.size()));
      for(int i = 0 ; i < l.size(); ++i)
      {
        m_md->serializeMessage(l[i].toMap(), _stream);
      }
      break;
    }
    default:
    {
      QVariantList l = _variant.toList();
      int i = 0;
      for(; i < std::min(l.size(), count()); ++i)
      {
        m_md->serializeMessage(l[i].toMap(), _stream);
      }
      for(; i < count(); ++i)
      {
        m_md->serializeMessage(QVariantMap(), _stream);
      }
      break;
    }
  }
}
void MessageMessageField::serializedLength(ros::serialization::LStream& _stream, const QVariant & _variant) const
{
  switch(count())
  {
    case 1:
      m_md->serializedLength(_variant.toMap(), _stream);
      break;
    case -1:
    {
      _stream.next(4);
      QVariantList l = _variant.toList();
      for(int i = 0 ; i < l.size(); ++i)
      {
        m_md->serializedLength(l[i].toMap(), _stream);
      }
      break;
    }
    default:
    {
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
      break;
    }
  }
}
