#include <QObject>

class AbstractMessageField;

namespace ros
{
  namespace serialization
  {
    class IStream;
    class OStream;
  }
}

class MessageDefinition : public QObject
{
  Q_OBJECT
public:
  MessageDefinition(const QString& _type_name);
  ~MessageDefinition();
  static MessageDefinition* get(const QString& _type_name);
  bool isValid() const { return m_valid; }
  QVariantMap parse(const QByteArray& _buffer) const;
  QVariantMap parse(ros::serialization::IStream& _stream) const;
  QByteArray generate(const QVariantMap& _hash) const;
  void generate(const QVariantMap& _hash, ros::serialization::OStream& _stream) const;
private:
  QString m_type_name;
  bool m_valid = false;
  QList<AbstractMessageField*> m_fields;
};
