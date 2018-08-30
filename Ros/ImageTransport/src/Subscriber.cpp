#include "Subscriber.h"

#include <QDebug>
#include <QQmlEngine>

#include <sensor_msgs/image_encodings.h>

#include "Image.h"

namespace
{

  struct ChannelsDescription
  {
    Image::Depth depth;
    quint8 channels;
    Image::Colorspace colorspace;
  };

  QHash<QString, ChannelsDescription> ros_to_channels_description;
}

Subscriber::Subscriber(QObject* _parent) : QObject(_parent), m_queue_size(1), m_skip(0), m_skipCount(0), m_imageTransport(m_handle)
{
  if(ros_to_channels_description.isEmpty())
  {
#define RRCD(__name__, __depth__, __channels__, __colorspace__) \
      ros_to_channels_description[QString::fromStdString(sensor_msgs::image_encodings::__name__)] = ChannelsDescription{Image::Depth::__depth__, __channels__, Image::Colorspace::__colorspace__};
    RRCD(RGB8, UInt8, 3, RGB);
    RRCD(RGBA8, UInt8, 4, RGBA);
    RRCD(RGB16, UInt16, 3, RGB);
    RRCD(RGBA16, UInt16, 4, RGBA);
    RRCD(BGR8, UInt8, 3, BGR);
    RRCD(BGRA8, UInt8, 4, BGRA);
    RRCD(BGR16, UInt16, 3, BGR);
    RRCD(BGRA16, UInt16, 4, BGRA);
    RRCD(MONO8, UInt8, 1, Mono);
    RRCD(MONO16, UInt16, 1, Mono);

    // OpenCV CvMat types
    RRCD(TYPE_8UC1, UInt8, 1, Unknown);
    RRCD(TYPE_8UC2, UInt8, 2, Unknown);
    RRCD(TYPE_8UC3, UInt8, 3, Unknown);
    RRCD(TYPE_8UC4, UInt8, 4, Unknown);
    RRCD(TYPE_8SC1, Int8, 1, Unknown);
    RRCD(TYPE_8SC2, Int8, 2, Unknown);
    RRCD(TYPE_8SC3, Int8, 3, Unknown);
    RRCD(TYPE_8SC4, Int8, 4, Unknown);
    RRCD(TYPE_16UC1, UInt16, 1, Unknown);
    RRCD(TYPE_16UC2, UInt16, 2, Unknown);
    RRCD(TYPE_16UC3, UInt16, 3, Unknown);
    RRCD(TYPE_16UC4, UInt16, 4, Unknown);
    RRCD(TYPE_16SC1, Int16, 1, Unknown);
    RRCD(TYPE_16SC2, Int16, 2, Unknown);
    RRCD(TYPE_16SC3, Int16, 3, Unknown);
    RRCD(TYPE_16SC4, Int16, 4, Unknown);
    RRCD(TYPE_32SC1, Int32, 1, Unknown);
    RRCD(TYPE_32SC2, Int32, 2, Unknown);
    RRCD(TYPE_32SC3, Int32, 3, Unknown);
    RRCD(TYPE_32SC4, Int32, 4, Unknown);
    RRCD(TYPE_32FC1, Float32, 1, Unknown);
    RRCD(TYPE_32FC2, Float32, 2, Unknown);
    RRCD(TYPE_32FC3, Float32, 3, Unknown);
    RRCD(TYPE_32FC4, Float32, 4, Unknown);
    RRCD(TYPE_64FC1, Float64, 1, Unknown);
    RRCD(TYPE_64FC2, Float64, 2, Unknown);
    RRCD(TYPE_64FC3, Float64, 3, Unknown);
    RRCD(TYPE_64FC4, Float64, 4, Unknown);

    // Bayer encodings
    RRCD(BAYER_RGGB8, UInt8, 1, BayerRGGB);
    RRCD(BAYER_BGGR8, UInt8, 1, BayerBGGR);
    RRCD(BAYER_GBRG8, UInt8, 1, BayerGBRG);
    RRCD(BAYER_GRBG8, UInt8, 1, BayerGRBG);
    RRCD(BAYER_RGGB16, UInt16, 1, BayerRGGB);
    RRCD(BAYER_BGGR16, UInt16, 1, BayerBGGR);
    RRCD(BAYER_GBRG16, UInt16, 1, BayerGBRG);
    RRCD(BAYER_GRBG16, UInt16, 1, BayerGRBG);
  }
}

Subscriber::~Subscriber()
{
}

void Subscriber::setTopicName(const QString& _topicName)
{
  m_topic_name = _topicName;
  emit(topicNameChanged());
  subscribe();
}

void Subscriber::setQueueSize(int _qS)
{
  m_queue_size = _qS;
  emit(queueSizeChanged());
  subscribe();
}

void Subscriber::callback(const sensor_msgs::ImageConstPtr& msg)
{
  if(m_skipCount < m_skip)
  {
    ++m_skipCount;
    return;
  }
  m_skipCount = 0;

  QString encodings = QString::fromStdString(msg->encoding);
  if(not ros_to_channels_description.contains(encodings))
  {
    qWarning() << "Unknown image encodings: " << encodings;
  }

  Image::Data data;
  
  data.width  = msg->width;
  data.height = msg->height;
  data.step   = msg->step;
  
  data.data.resize(msg->data.size());
  std::memcpy(data.data.data(), msg->data.data(), msg->data.size());
  
  ChannelsDescription cd = ros_to_channels_description.value(encodings);
  
  data.depth      = cd.depth;
  data.channels   = cd.channels;
  data.colorspace = cd.colorspace;
  
  if(m_lastImage) m_lastImage->setParent(nullptr);
  m_lastImage = new Image(data);
  m_lastImage->moveToThread(thread()); // Move to the QML engine thread
  m_lastImage->setParent(this);
  QQmlEngine::setObjectOwnership(m_lastImage, QQmlEngine::JavaScriptOwnership);
  emit(imageReceived(m_lastImage));
}

void Subscriber::subscribe()
{
  if(m_topic_name.isEmpty() or m_queue_size <= 0)
  {
    m_subscriber = image_transport::Subscriber();
  } else {
    m_subscriber = m_imageTransport.subscribe(m_topic_name.toStdString().c_str(), m_queue_size, &Subscriber::callback, this);
  }
}
