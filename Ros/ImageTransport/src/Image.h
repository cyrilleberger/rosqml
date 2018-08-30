#include <QObject>

class QImage;

class Image : public QObject
{
  Q_OBJECT
public:
  enum class Colorspace
  {
    Unknown,
    RGB,
    RGBA,
    BGR,
    BGRA,
    Mono,
    BayerRGGB,
    BayerBGGR,
    BayerGBRG,
    BayerGRBG
  };
  Q_ENUM(Colorspace)
  enum class Depth
  {
    UInt8,
    Int8,
    UInt16,
    Int16,
    UInt32,
    Int32,
    UInt64,
    Int64,
    Float32,
    Float64
  };
  Q_ENUM(Depth)
public:
  struct Data
  {
    QByteArray data;
    quint32 width;
    quint32 height;
    quint32 step;
    Image::Depth depth;
    quint8 channels;
    Image::Colorspace colorspace;
    bool isEmpty() const
    {
      return width == 0 or height == 0 or data.size() == 0;
    }
  };
public:
  Q_PROPERTY(Colorspace colorspace  READ colorspace NOTIFY imageDataChanged)
  Q_PROPERTY(Depth      depth       READ depth      NOTIFY imageDataChanged)
  Q_PROPERTY(quint32    width       READ width      NOTIFY imageDataChanged)
  Q_PROPERTY(quint32    height      READ height     NOTIFY imageDataChanged)
  Q_PROPERTY(quint8     channels    READ channels   NOTIFY imageDataChanged)
public:
  Image(QObject* _parent = nullptr) : QObject(_parent) {}
  Image(const Data& _data, QObject* _parent = nullptr) : QObject(_parent), m_imageData(_data) {}
  ~Image() {}
  Depth depth() const { return m_imageData.depth; }
  Colorspace colorspace() const { return m_imageData.colorspace; }
  quint32 width() const { return m_imageData.width; }
  quint32 height() const { return m_imageData.height; }
  quint8 channels() const { return m_imageData.channels; }
  void setImageData(const Data& _imageData);
  const Data& imageData() const { return m_imageData; }
  QImage toQImage() const;
signals:
  void imageDataChanged();
private:
  Data m_imageData;
};
