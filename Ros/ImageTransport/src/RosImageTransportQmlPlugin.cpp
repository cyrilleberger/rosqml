#include "RosImageTransportQmlPlugin.h"

#include <QtQml>

#include "Image.h"
#include "ImageView.h"
#include "Publisher.h"
#include "Subscriber.h"

RosImageTransportQmlPlugin::RosImageTransportQmlPlugin()
{
}

void RosImageTransportQmlPlugin::registerTypes(const char *uri)
{
  qmlRegisterType<Publisher>  (uri, 1, 0, "Publisher");
  qmlRegisterType<Subscriber> (uri, 1, 0, "Subscriber");
  qmlRegisterType<Image>      (uri, 1, 0, "Image");
  qmlRegisterType<ImageView>  (uri, 1, 0, "ImageView");
}
