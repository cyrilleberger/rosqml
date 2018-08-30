#include "RosImageTransportQmlPlugin.h"

#include <QtQml>

#include "Image.h"
#include "ImageViewItem.h"
#include "Subscriber.h"

RosImageTransportQmlPlugin::RosImageTransportQmlPlugin()
{
}

void RosImageTransportQmlPlugin::registerTypes(const char *uri)
{
  qmlRegisterType<Subscriber>     (uri, 1, 0, "Subscriber");
  qmlRegisterType<Image>          (uri, 1, 0, "Image");
  qmlRegisterType<ImageViewItem>  (uri, 1, 0, "ImageView");
}
