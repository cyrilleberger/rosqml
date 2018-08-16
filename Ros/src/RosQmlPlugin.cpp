#include "RosQmlPlugin.h"

#include <QtQml>

#include "MessageDefinition.h"
#include "Publisher.h"
#include "RosThread.h"
#include "ServiceClient.h"
#include "Subscriber.h"
#include "RosWrapper.h"

RosQmlPlugin::RosQmlPlugin()
{
  qRegisterMetaType<MessageDefinition*>("MessageDefinition*");
}

void RosQmlPlugin::registerTypes(const char *uri)
{
  qmlRegisterType<Subscriber>(uri, 1, 0, "Subscriber");
  qmlRegisterType<Publisher>(uri, 1, 0, "Publisher");
  qmlRegisterType<ServiceClient>(uri, 1, 0, "ServiceClient");
  qmlRegisterSingletonType(uri, 1, 0, "Ros", [] (QQmlEngine */*engine*/, QJSEngine *scriptEngine) -> QJSValue
      {
        return scriptEngine->newQObject(new RosWrapper);
      });
}

void RosQmlPlugin::initializeEngine(QQmlEngine */*engine*/, const char */*uri*/)
{
  RosThread::instance()->start();
}
