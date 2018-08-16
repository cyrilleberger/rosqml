#include "MessageDefinition.h"

#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QJSValue>
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
  static QString suffix(int _count)
  {
    if(_count == 1)
    {
      return QString();
    } else {
      return QString("[%1]").arg(_count);
    }
  }
public:
  BaseTypeMessageField(const QString _name, Type _type, int _count) : MessageField(_name + suffix(_count), _type, _count)
  {
  }
  QVariant deserialize(ros::serialization::IStream& _stream) const override
  {
    if(count() == 1)
    {
      _T_ v;
      _stream.next(v);
      return to_variant<_T_>(v);
    } else {
      QVariantList l;
      for(int i = 0; i < count(); ++i)
      {
        _T_ v;
        _stream.next(v);
        l.append(to_variant<_T_>(v));
      }
      return l;
    }
  }
  void serialize(ros::serialization::OStream & _stream, const QVariant & _variant) const override
  {
    if(count() == 1)
    {
      _stream.next(from_variant<_T_>(_variant));
    } else {
      QVariantList l = _variant.toList();
      int i = 0;
      for(; i < std::min(l.size(), count()); ++i)
      {
        _stream.next(from_variant<_T_>(l[i]));
      }
      for(; i < count(); ++i)
      {
        _T_ v;
        _stream.next(v);
      }
    }
  }
  void serializedLength(ros::serialization::LStream& _stream, const QVariant & _variant) const override
  {
    if(count() == 1)
    {
      _stream.next(from_variant<_T_>(_variant));
    } else {
      QVariantList l = _variant.toList();
      int i = 0;
      for(; i < std::min(l.size(), count()); ++i)
      {
        _stream.next(from_variant<_T_>(l[i]));
      }
      for(; i < count(); ++i)
      {
        _T_ v;
        _stream.next(v);
      }
    }
  }
};

MessageDefinition::MessageDefinition(QObject* _parent) : QObject(_parent)
{
}

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
  const QString packagename = splited[0];
  const QString messagename = splited[1];
  QFile file(QString::fromStdString(ros::package::getPath(packagename.toStdString()).c_str()) + "/msg/" + messagename + ".msg");
  if(file.open(QIODevice::ReadOnly))
  {
    QTextStream stream(&file);
    parseDefinition(packagename, stream);
  } else {
    qWarning() << "Failed to open: " << file.fileName();
  }
}

void MessageDefinition::parseDefinition(const QString& _packagename, QTextStream& _stream)
{
  QString md5text, md5constants; // see https://github.com/ros/genmsg/blob/indigo-devel/src/genmsg/gentools.py for computation of md5
  m_valid = true;
  QString subdefinition;
  while(not _stream.atEnd())
  {
    QString line = _stream.readLine();
    m_definition += line;
    int comment_char = line.indexOf('#');
    QStringRef ref = (comment_char >= 0) ? line.leftRef(comment_char) : QStringRef(&line);
    QVector<QStringRef>  l = ref.split(' ', QString::SkipEmptyParts);
    if(l.size() == 2)
    {
      QString type = l[0].toString();
      QString md5type = type;
      QString name = l[1].toString();
      int count = 1;
      if(type.endsWith("[]"))
      {
        qFatal("unimplemented");
      }
      QString baseType;
      if(type.endsWith(']'))
      {
        QRegExp r("(.*)\\[(.*)\\]");
        r.exactMatch(type);
        baseType = r.cap(1);
        count    = r.cap(2).toInt();
        qDebug() << baseType << count;
      } else {
        baseType = type;
      }
      if(baseType == "string")
      {
        m_fields.append(new BaseTypeMessageField<std::string>(name, MessageField::Type::String, count));
      } else if(baseType == "float32")
      {
        m_fields.append(new BaseTypeMessageField<float>(name, MessageField::Type::Float32, count));
      } else if(baseType == "float64")
      {
        m_fields.append(new BaseTypeMessageField<double>(name, MessageField::Type::Float64, count));
      } else if(baseType == "uint8")
      {
        m_fields.append(new BaseTypeMessageField<quint8>(name, MessageField::Type::UInt8, count));
      } else if(baseType == "int8")
      {
        m_fields.append(new BaseTypeMessageField<qint8>(name, MessageField::Type::Int8, count));
      } else if(baseType == "uint16")
      {
        m_fields.append(new BaseTypeMessageField<quint16>(name, MessageField::Type::UInt16, count));
      } else if(baseType == "int16")
      {
        m_fields.append(new BaseTypeMessageField<qint16>(name, MessageField::Type::Int16, count));
      } else if(baseType == "uint32")
      {
        m_fields.append(new BaseTypeMessageField<quint32>(name, MessageField::Type::UInt32, count));
      } else if(baseType == "int32")
      {
        m_fields.append(new BaseTypeMessageField<qint32>(name, MessageField::Type::Int32, count));
      } else if(baseType == "uint64")
      {
        m_fields.append(new BaseTypeMessageField<uint64_t>(name, MessageField::Type::UInt64, count));
      } else if(baseType == "int64")
      {
        m_fields.append(new BaseTypeMessageField<int64_t>(name, MessageField::Type::Int64, count));
      }  else if(baseType == "time")
      {
        m_fields.append(new BaseTypeMessageField<ros::Time>(name, MessageField::Type::Time, count));
      } else {
        if(baseType == "Header")
        {
          baseType = "std_msgs/Header";
          type = "std_msgs/" + type;
        }
        if(not type.contains("/"))
        {
          type = _packagename + "/" + type;
        }
        MessageDefinition* md = MessageDefinition::get(type);
        qDebug() << md << type << md->isValid();
        if(md->isValid())
        {
          m_fields.append(new MessageMessageField(name, md, count));
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
  m_definition += subdefinition;
  md5text.chop(1);
  m_md5_definition = md5constants + md5text;
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

QVariantMap MessageDefinition::variantToMap(const QVariant& _variant) const
{
  if(_variant.canConvert<QJSValue>())
  {
    QJSValue value = _variant.value<QJSValue>();
    if(value.isArray())
    {
      QVariantList list;
      const int length = value.property("length").toInt();
      for(int i = 0; i < length; ++i)
      {
        list.append(value.property(i).toVariant());
      }
      return variantToMap(list);
    } else if(value.isObject()) {
      return value.toVariant().toMap();
    } else {
      QVariantList list;
      list.append(value.toVariant());
      return variantToMap(list);
    }
  } else if(_variant.canConvert<QVariantMap>())
  {
    return  _variant.toMap();
  } else if(_variant.canConvert<QVariantList>())
  {
    QVariantList list = _variant.toList();
    QVariantMap r;
    int i = 0;
    for(; i < std::min(list.size(), m_fields.size()); ++i)
    {
      const MessageField* mf = m_fields[i];
      QVariant value = list[i];
      if(mf->type() == MessageField::Type::Message)
      {
        const MessageMessageField* mmf = qobject_cast<const MessageMessageField*>(mf);
        r[mf->name()] = mmf->messageDefinition()->variantToMap(value);
      } else {
        r[mf->name()] = value;
      }
    }
    return r;
  } else if(m_fields.size() == 1) {
    QVariantMap r;
    const MessageField* mf = m_fields.first();
    if(mf->type() == MessageField::Type::Message)
    {
      const MessageMessageField* mmf = qobject_cast<const MessageMessageField*>(mf);
      r[mf->name()] = mmf->messageDefinition()->variantToMap(_variant);
    } else {
      r[mf->name()] = _variant;
    }
    return r;
  } else {
    qWarning() << "Invalid message: " << _variant;
    return QVariantMap();
  }
}

QVariantMap MessageDefinition::deserializeMessage(const QByteArray& _buffer) const
{
  ros::serialization::IStream stream(const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(_buffer.data())), _buffer.size());
  return deserializeMessage(stream);
}

QVariantMap MessageDefinition::deserializeMessage(ros::serialization::IStream& _stream) const
{
  QVariantMap r;

  for(MessageField* field : m_fields)
  {
    r[field->name()] = field->deserialize(_stream);
  }
  
  return r;
}

QByteArray MessageDefinition::serializeMessage(const QVariantMap& _hash) const
{
  quint32 sl = serializedLength(_hash);
  QByteArray arr;
  arr.resize(sl);
  
  ros::serialization::OStream stream(reinterpret_cast<quint8*>(arr.data()), arr.size());
  
  serializeMessage(_hash, stream);
  
  return arr;
}

void MessageDefinition::serializeMessage(const QVariantMap& _hash, ros::serialization::OStream& _stream) const
{
  for(MessageField* field : m_fields)
  {
    field->serialize(_stream, _hash[field->name()]);
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
