#include <QObject>

namespace ros
{
  namespace serialization
  {
    class IStream;
    class OStream;
    class LStream;
  }
}

class MessageField : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name CONSTANT)
  Q_PROPERTY(Type type READ type CONSTANT)
public:
  enum class Type {
    Bool, Int8, UInt8, Int16, UInt16, Int32, UInt32, Int64, UInt64, Float32, Float64, String, Time, Duration, Message
  };
  Q_ENUM(Type)
public:
  MessageField(const QString& _name, Type _type, int _count) : m_name(_name), m_type(_type), m_count(_count)
  {
  }
  QString name() const { return m_name; }
  Type type() const { return m_type; }
  int count() const { return m_count; }
  virtual QVariant deserialize(ros::serialization::IStream& _stream) const = 0;
  virtual void serialize(ros::serialization::OStream& _stream, const QVariant& _variant) const = 0;
  virtual void serializedLength(ros::serialization::LStream& _stream, const QVariant& _variant) const = 0;
private:
  QString m_name;
  Type m_type;
  int m_count;
};

