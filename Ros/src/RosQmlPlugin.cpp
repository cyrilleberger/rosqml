#include "RosQmlPlugin.h"

#include <QtQml>

#include "MessageDefinition.h"
#include "Publisher.h"
#include "RosThread.h"
#include "Subscriber.h"

RosQmlPlugin::RosQmlPlugin()
{
  qRegisterMetaType<MessageDefinition*>("MessageDefinition*");
}

void RosQmlPlugin::registerTypes(const char *uri)
{
  qmlRegisterType<Subscriber>(uri, 1, 0, "Subscriber");
  qmlRegisterType<Publisher>(uri, 1, 0, "Publisher");
}

void RosQmlPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
  RosThread::instance()->start();
}
