#include <QObject>

#include <image_transport/image_transport.h>

class Image;

class Subscriber : public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString topicName READ topicName WRITE setTopicName NOTIFY topicNameChanged)
  Q_PROPERTY(int queueSize READ queueSize WRITE setQueueSize NOTIFY queueSizeChanged)
  Q_PROPERTY(int skip READ skip WRITE setSkip NOTIFY skipChanged)
  Q_PROPERTY(Image* lastImage READ lastImage NOTIFY imageReceived);
public:
  Subscriber(QObject* _parent = nullptr);
  ~Subscriber();
  Image* lastImage() const { return m_lastImage; }
  QString topicName() const { return m_topic_name; }
  void setTopicName(const QString& _topicName);
  int queueSize() const { return m_queue_size; }
  int skip() const { return m_skip; }
  void setSkip(int skip) { m_skip = skip; m_skipCount = m_skip; emit(skipChanged()); }
  void setQueueSize(int _qS);
signals:
  void topicNameChanged();
  void queueSizeChanged();
  void skipChanged();
  void imageReceived(Image* image);
private:
  void callback(const sensor_msgs::ImageConstPtr& msg);
  void subscribe();
  ros::NodeHandle m_handle;
  QString m_topic_name;
  int m_queue_size;
  int m_skip, m_skipCount;
  image_transport::ImageTransport m_imageTransport;
  image_transport::Subscriber m_subscriber;
  Image* m_lastImage;
};
