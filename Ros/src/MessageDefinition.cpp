#include "MessageDefinition.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QHash>
#include <QTextStream>

#include <ros/package.h>
#include <ros/serialization.h>

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
  template<typename _T_>
  inline _T_ from_variant(const QVariant& _v)
  {
    return _v.value<_T_>();
  }
  template<>
  inline std::string from_variant<std::string>(const QVariant& _v)
  {
    return _v.value<QString>().toStdString();
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
  virtual void generate(ros::serialization::OStream& _stream, const QVariant& _variant) const = 0;
  virtual void serializedLength(ros::serialization::LStream& _stream, const QVariant& _variant) const = 0;
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
  QVariant parse(ros::serialization::IStream& _stream) const override
  {
    _T_ v;
    _stream.next(v);
    return to_variant<_T_>(v);
  }
  void generate(ros::serialization::OStream & _stream, const QVariant & _variant) const override
  {
    _stream.next(from_variant<_T_>(_variant));
  }
  void serializedLength(ros::serialization::LStream& _stream, const QVariant & _variant) const override
  {
    _stream.next(from_variant<_T_>(_variant));
  }
};

class MessageMessageField : public AbstractMessageField
{
public:
  MessageMessageField(const QString _name, MessageDefinition* _md) : AbstractMessageField(_name), m_md(_md)
  {
  }
  QVariant parse(ros::serialization::IStream& _stream) const override
  {
    return m_md->parse(_stream);
  }
  void generate(ros::serialization::OStream & _stream, const QVariant & _variant) const override
  {
    m_md->generate(_variant.toMap(), _stream);
  }
  void serializedLength(ros::serialization::LStream& _stream, const QVariant & _variant) const override
  {
    m_md->serializedLength(_variant.toMap(), _stream);
  }
private:
  MessageDefinition* m_md;
};

MessageDefinition::MessageDefinition(const QString& _type_name) : m_type_name(_type_name)
{
  QStringList splited = m_type_name.split('/');
  QString subdefinition;
  QString md5text; // see https://github.com/ros/genmsg/blob/indigo-devel/src/genmsg/gentools.py for computation of md5
  QFile file(QString::fromStdString(ros::package::getPath(splited[0].toStdString()).c_str()) + "/msg/" + splited[1] + ".msg");
  if(file.open(QIODevice::ReadOnly))
  {
    m_valid = true;
    QTextStream stream(&file);
    while(not stream.atEnd())
    {
      QString line = stream.readLine();
      m_definition += line;
      int comment_char = line.indexOf('#');
      QStringRef ref = (comment_char >= 0) ? line.leftRef(comment_char) : QStringRef(&line);
      QVector<QStringRef>  l = ref.split(' ', QString::SkipEmptyParts);
      if(l.size() == 2)
      {
        QString type = l[0].toString();
        QString md5type = type;
        QString name = l[1].toString();
//         QString
        if(type == "string")
        {
          m_fields.append(new MessageField<std::string>(name));
        } else {
          MessageDefinition* md = MessageDefinition::get(type);
          if(md->isValid())
          {
            m_fields.append(new MessageMessageField(name, md));
            md5type = QString::fromUtf8(md->md5());
            subdefinition += "================================================================================\n\
MSG: " + md->typeName() + "\n" + md->definition();
          } else {
            qWarning() << "Unsupported field type: " << type << name;
            m_valid = false;
          }
        }
        md5text += "string " + name + "\n";
      } else if(l.size() != 0) {
        qWarning() << "Invalid line: " << line;
        m_valid = false;
      }
    }
  } else {
    qWarning() << "Failed to open: " << file.fileName();
  }
  m_definition += subdefinition;
  md5text.chop(1);
  m_md5 = QCryptographicHash::hash(md5text.toUtf8(), QCryptographicHash::Md5);
  qDebug() << "Hash for " << m_type_name << " is " << m_md5.toHex() << " text " << md5text;
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
    if(_type_name == "Header")
    {
      definitions["Header"] = md;
    }
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
  quint32 sl = serializedLength(_hash);
  QByteArray arr;
  arr.resize(sl);
  
  ros::serialization::OStream stream(reinterpret_cast<quint8*>(arr.data()), arr.size());
  
  generate(_hash, stream);
  
  return arr;
}

void MessageDefinition::generate(const QVariantMap& _hash, ros::serialization::OStream& _stream) const
{
  for(AbstractMessageField* field : m_fields)
  {
    field->generate(_stream, _hash[field->name()]);
  }
}

quint32 MessageDefinition::serializedLength(const QVariantMap& _map) const
{
  ros::serialization::LStream stream;
  serializedLength(_map, stream);
  return stream.getLength();
}

void MessageDefinition::serializedLength(const QVariantMap& _map, ros::serialization::LStream& _stream) const
{
  for(AbstractMessageField* field : m_fields)
  {
    field->serializedLength(_stream, _map[field->name()]);
  }
}


