#include "RosObject.h"

#include <ros/service_client.h>

class ServiceDefinition;

class ServiceClient : public RosObject
{
  Q_OBJECT
  Q_PROPERTY(bool callInProgress READ callInProgress NOTIFY callInProgressChanged)
  Q_PROPERTY(QString dataType READ dataType WRITE setDataType NOTIFY dataTypeChanged)
  Q_PROPERTY(QString serviceName READ serviceName WRITE setServiceName NOTIFY serviceNameChanged)
  Q_PROPERTY(ServiceDefinition* serviceDefinition READ serviceDefinition NOTIFY serviceDefinitionChanged)
public:
  ServiceClient(QObject* _parent = nullptr);
  ~ServiceClient();
  QString serviceName() const { return m_service_name; }
  void setServiceName(const QString& _serviceName);
  QString dataType() const { return m_data_type; }
  void setDataType(const QString& _serviceName);
  bool callInProgress() const { return m_called; }
  Q_INVOKABLE bool call(const QVariant& _message);
  ServiceDefinition* serviceDefinition() const { return m_service_definition; }
signals:
  void dataTypeChanged();
  void serviceNameChanged();
  void serviceDefinitionChanged();
  void callInProgressChanged();
  void answerReceived(const QVariant& answer);
  void callFailed();
private:
  void start_client();
  QString m_service_name, m_data_type;
  ros::ServiceClient m_client;
  ServiceDefinition* m_service_definition = nullptr;
  bool m_called = false;
};
