#include "RosWrapper.h"

#include <QDateTime>
#include <QDebug>
#include <QVariant>

#include "RosThread.h"

RosWrapper::RosWrapper(QObject* _parent) : RosObject(_parent)
{
}

RosWrapper::~RosWrapper()
{
}

quint64 RosWrapper::startTime() const
{
  return RosThread::instance()->startTime();
}

quint64 RosWrapper::now() const
{
  return RosThread::instance()->now();
}

namespace
{
  QVariant xmlRpcToVariant(XmlRpc::XmlRpcValue _value)
  {
    switch(_value.getType())
    {
      default:
      case XmlRpc::XmlRpcValue::TypeInvalid:
      {
        return QVariant();
      }
      case XmlRpc::XmlRpcValue::TypeBoolean:
      {
        return QVariant((bool)_value);
      }
      case XmlRpc::XmlRpcValue::TypeInt:
      {
        return QVariant((int)_value);
      }
      case XmlRpc::XmlRpcValue::TypeDouble:
      {
        return QVariant((double)_value);
      }
      case XmlRpc::XmlRpcValue::TypeString:
      {
        return QVariant(QString::fromStdString(_value));
      }
      case XmlRpc::XmlRpcValue::TypeDateTime:
      {
        tm t = _value;
        return QVariant(QDateTime::fromTime_t(std::mktime(&t)));
      }
      case XmlRpc::XmlRpcValue::TypeBase64:
      {
        XmlRpc::XmlRpcValue::BinaryData bd = _value;
        return QByteArray(reinterpret_cast<const char*>(bd.data()), bd.size());
      }
      case XmlRpc::XmlRpcValue::TypeArray:
      {
        QVariantList var;
        for(int i = 0; i < _value.size(); ++i)
        {
          var.append(xmlRpcToVariant(_value[i]));
        }
        return var;
      }
      case XmlRpc::XmlRpcValue::TypeStruct:
      {
        QVariantMap map;
        for(XmlRpc::XmlRpcValue::iterator it = _value.begin(); it != _value.end(); ++it)
        {
          map[QString::fromStdString(it->first)] = xmlRpcToVariant(it->second);
        }
        return map;
      }
    }
  }
}

QVariant RosWrapper::getParam(const QString& _name) const
{
  if(_name.startsWith('~'))
  {
    ros::NodeHandle handle("~");
    std::string str;
    XmlRpc::XmlRpcValue value;
    if(handle.getParam(_name.right(_name.length() - 1).toStdString(), value))
    {
      return xmlRpcToVariant(value);
    } else {
      qWarning() << "No parameter called: " << _name;
      return QVariant();
    }
  } else {
    XmlRpc::XmlRpcValue value;
    if(m_handle.getParam(_name.toStdString(), value))
    {
      return xmlRpcToVariant(value);
    } else {
      qWarning() << "No parameter called: " << _name;
      return QVariant();
    }
  }
}

#include "moc_RosWrapper.cpp"
