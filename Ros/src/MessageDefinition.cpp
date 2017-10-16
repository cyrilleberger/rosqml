#include "MessageDefinition.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QHash>
#include <QTextStream>

#include <ros/package.h>
#include <ros/serialization.h>
#include <ros/time.h>

#include "MessageMessageField.h"

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
  template<>
  inline QVariant to_variant<ros::Time>(const ros::Time& _v)
  {
    QVariantMap map;
    map["sec"] = _v.sec;
    map["nsec"] = _v.nsec;
    return map;
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
  template<>
  inline ros::Time from_variant<ros::Time>(const QVariant& _v)
  {
    QVariantMap v = _v.toMap();
    return ros::Time(v["sec"].toInt(), v["nsec"].toInt());
  }
}

template<typename _T_>
class BaseTypeMessageField : public MessageField
{
public:
  BaseTypeMessageField(const QString _name, Type _type) : MessageField(_name, _type)
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

MessageDefinition::MessageDefinition(const QString& _type_name) : m_type_name(_type_name)
{
  qDebug() << "MessageDefinition for " << m_type_name;
  if(m_type_name == "Header")
  {
    m_type_name = "std_msgs/Header";
  }

  QStringList splited = m_type_name.split('/');
  if(splited.size() != 2)
  {
    qWarning() << "Invalid type: " << _type_name;
    return;
  }
  QString subdefinition;
  QString md5text, md5constants; // see https://github.com/ros/genmsg/blob/indigo-devel/src/genmsg/gentools.py for computation of md5
  const QString packagename = splited[0];
  const QString messagename = splited[1];
  QFile file(QString::fromStdString(ros::package::getPath(packagename.toStdString()).c_str()) + "/msg/" + messagename + ".msg");
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
        if(type == "string")
        {
          m_fields.append(new BaseTypeMessageField<std::string>(name, MessageField::Type::String));
        } else if(type == "float32")
        {
          m_fields.append(new BaseTypeMessageField<float>(name, MessageField::Type::Float32));
        } else if(type == "float64")
        {
          m_fields.append(new BaseTypeMessageField<double>(name, MessageField::Type::Float64));
        } else if(type == "uint8")
        {
          m_fields.append(new BaseTypeMessageField<quint8>(name, MessageField::Type::UInt8));
        } else if(type == "int8")
        {
          m_fields.append(new BaseTypeMessageField<qint8>(name, MessageField::Type::Int8));
        } else if(type == "uint16")
        {
          m_fields.append(new BaseTypeMessageField<quint16>(name, MessageField::Type::UInt16));
        } else if(type == "int16")
        {
          m_fields.append(new BaseTypeMessageField<qint16>(name, MessageField::Type::Int16));
        } else if(type == "uint32")
        {
          m_fields.append(new BaseTypeMessageField<quint32>(name, MessageField::Type::UInt32));
        } else if(type == "int32")
        {
          m_fields.append(new BaseTypeMessageField<qint32>(name, MessageField::Type::Int32));
        }  else if(type == "time")
        {
          m_fields.append(new BaseTypeMessageField<ros::Time>(name, MessageField::Type::Time));
        } else {
          if(type == "Header")
          {
            type = "std_msgs/Header";
          }
          if(not type.contains("/"))
          {
            type = packagename + "/" + type;
          }
          MessageDefinition* md = MessageDefinition::get(type);
          qDebug() << md << type << md->isValid();
          if(md->isValid())
          {
            m_fields.append(new MessageMessageField(name, md));
            md5type = QString::fromUtf8(md->md5().toHex());
            subdefinition += "================================================================================\n\
MSG: " + md->typeName() + "\n" + md->definition();
          } else {
            qWarning() << "Unsupported field type: " << type << name;
            m_valid = false;
          }
        }
        md5text += md5type + " " + name + "\n";
      } else if(l.size() == 4) {
        if(l[2] == "=")
        {
          md5constants += l[0].toString() + " " + l[1].toString() + " = " + l[3].toString() + "\n";
        } else {
          qWarning() << "Invalid line: " << line;
          m_valid = false;
        }
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
  m_md5 = QCryptographicHash::hash((md5constants + md5text).toUtf8(), QCryptographicHash::Md5);
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
      definitions["std_msgs/Header"] = md;
    } else if(_type_name == "std_msgs/Header")
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

  for(MessageField* field : m_fields)
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
  for(MessageField* field : m_fields)
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
  for(MessageField* field : m_fields)
  {
    field->serializedLength(_stream, _map[field->name()]);
  }
}

QList<QObject*> MessageDefinition::fieldsLO() const
{
  return *reinterpret_cast<const QList<QObject*>*>(&m_fields);
}
