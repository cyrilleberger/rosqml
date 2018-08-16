#include "ServiceClient.h"

#include <ros/serialization.h>

#include <QtConcurrent/QtConcurrent>

#include "MessageDefinition.h"
#include "ServiceDefinition.h"

ServiceClient::ServiceClient(QObject* _parent) : RosObject(_parent)
{
}

ServiceClient::~ServiceClient()
{
}

void ServiceClient::setDataType(const QString& _dataType)
{
  m_data_type = _dataType;
  emit(dataTypeChanged());
  start_client();
}

void ServiceClient::setServiceName(const QString& _serviceName)
{
  m_service_name = _serviceName;
  emit(serviceNameChanged());
  start_client();
}

bool ServiceClient::call(const QVariant& _message)
{
  if(not m_service_definition)
  {
    qWarning() << "Service definition not set or not found";
    return false;
  }
  if(m_called)
  {
    qWarning() << "Service call in progress";
    return false;
  }
  m_called = true;
  emit(callInProgressChanged());

  QVariantMap message = m_service_definition->requestDefinition()->variantToMap(_message);
  
  QtConcurrent::run([this, message]() {
    ros::SerializedMessage answer;
    
    quint32 request_length = m_service_definition->requestDefinition()->serializedLength(message) + 4;
    ros::SerializedMessage request(boost::shared_array<uint8_t>(new uint8_t[request_length]), request_length);

    ros::serialization::OStream s(request.buf.get(), request.num_bytes);
    ros::serialization::serialize(s, (uint32_t)request.num_bytes - 4);
    m_service_definition->requestDefinition()->serializeMessage(message, s);
        
    bool success = m_client.call(request, answer, m_service_definition->md5().toHex().toStdString());
        
    if(success)
    {
      QByteArray answerdata((const char*)answer.message_start, answer.num_bytes);
      QVariantMap h = m_service_definition->answerDefinition()->deserializeMessage(answerdata);
      emit(answerReceived(h));
    } else {
      emit(callFailed());
    }
    m_called = false;
    emit(callInProgressChanged());
  });
  return false;
}

void ServiceClient::start_client()
{
  if(m_data_type.isEmpty() or m_service_name.isEmpty())
  {
    m_client = ros::ServiceClient();
  } else {
    m_service_definition = ServiceDefinition::get(m_data_type);
    emit(serviceDefinitionChanged());
    ros::ServiceClientOptions sco(m_service_name.toStdString(), m_service_definition->md5().toHex().toStdString(), false, ros::M_string());
    m_client = m_handle.serviceClient(sco);
  }
}
