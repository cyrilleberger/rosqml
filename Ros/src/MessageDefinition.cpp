#include "MessageDefinition.h"

#include <QDebug>
#include <QFile>
#include <QHash>
#include <QTextStream>

#include <ros/package.h>

#include <std_msgs/String.h>

namespace {
  template<typename _T_>
  inline QVariant to_variant(const _T_& _v)
  {
    return QVariant::fromValue(_v);
  }
  template<>
  inline QVariant to_variant<std::string>(const std::string& _v)
  {
    return QVariant::fromValue(QString::fromStdString(_v));
  }
}

class AbstractMessageField
{
public:
  AbstractMessageField(const QString& _name) : m_name(_name)
  {
  }
  QString name() const { return m_name; }
  virtual QVariant parse(ros::serialization::IStream& _stream) const = 0;
private:
  QString m_name;
};

template<typename _T_>
class MessageField : public AbstractMessageField
{
public:
  MessageField(const QString _name) : AbstractMessageField(_name)
  {
  }
  virtual QVariant parse(ros::serialization::IStream& _stream) const
  {
    _T_ v;
    _stream.next(v);
    return to_variant<_T_>(v);
  }

};

class MessageMessageField : public AbstractMessageField
{
public:
  MessageMessageField(const QString _name, MessageDefinition* _md) : AbstractMessageField(_name), m_md(_md)
  {
  }
  virtual QVariant parse(ros::serialization::IStream& _stream) const
  {
    return m_md->parse(_stream);
  }
private:
  MessageDefinition* m_md;
};

MessageDefinition::MessageDefinition(const QString& _type_name)
{
  QStringList splited = _type_name.split('/');
  
  QFile file(QString::fromStdString(ros::package::getPath(splited[0].toStdString()).c_str()) + "/msg/" + splited[1] + ".msg");
  if(file.open(QIODevice::ReadOnly))
  {
    m_valid = true;
    QTextStream stream(&file);
    while(not stream.atEnd())
    {
      QString line = stream.readLine();
      int comment_char = line.indexOf('#');
      QStringRef ref = (comment_char >= 0) ? line.leftRef(comment_char) : QStringRef(&line);
      QVector<QStringRef>  l = ref.split(' ', QString::SkipEmptyParts);
      if(l.size() >= 2)
      {
        QStringRef type = l[0];
        QString name = l[1].toString();
//         QString
        if(type == "string")
        {
          m_fields.append(new MessageField<std::string>(name));
        } else {
          MessageDefinition* md = MessageDefinition::get(type.toString());
          if(md->isValid())
          {
            m_fields.append(new MessageMessageField(name, md));
          } else {
            qWarning() << "Unsupported field type: " << type << name;
            m_valid = false;
          }
        }
      } else if(l.size() == 1) {
        qWarning() << "Invalid line: " << line;
        m_valid = false;
      }
    }
    qDebug() << file.readAll();
  } else {
    qWarning() << "Failed to open: " << file.fileName();
  }
  
}

MessageDefinition::~MessageDefinition()
{
}

MessageDefinition* MessageDefinition::get(const QString& _type_name)
{
  static QHash<QString, MessageDefinition*> definitions;
  MessageDefinition* md = definitions[_type_name];
  if(not md)
  {
    md = new MessageDefinition(_type_name);
    definitions[_type_name] = md;
  }
  return md;
}



QVariantMap MessageDefinition::parse(const QByteArray& _buffer) const
{
  ros::serialization::IStream stream(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(_buffer.data())), _buffer.size());
  return parse(stream);
}

QVariantMap MessageDefinition::parse(ros::serialization::IStream& _stream) const
{
  QVariantMap r;

  for(AbstractMessageField* field : m_fields)
  {
    r[field->name()] = field->parse(_stream);
  }
  
  return r;
}

QByteArray MessageDefinition::generate(const QVariantMap& _hash) const
{
  return QByteArray();
}

