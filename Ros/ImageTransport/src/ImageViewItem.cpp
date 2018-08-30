#include "ImageViewItem.h"

#include <QQuickWindow>
#include <QSGSimpleTextureNode>
#include <QtConcurrent/QtConcurrent>

#include "Image.h"

ImageViewItem::ImageViewItem(QQuickItem* _parent) : QQuickItem(_parent), m_image(new Image)
{
  setFlag(QQuickItem::ItemHasContents);
  m_pool.setMaxThreadCount(1);
}

ImageViewItem::~ImageViewItem()
{
}

Image* ImageViewItem::image() const
{
  return m_image;
}

void ImageViewItem::setImage(Image* _image)
{
  if(_image)
  {
    Image::Data data = _image->imageData();
    m_image->setImageData(data);
    m_pool.clear();
    QtConcurrent::run(&m_pool, [this, data](){
      m_img = Image(data).toQImage();
      QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
    });
  } else {
    m_image->setImageData(Image::Data());
    m_img = QImage();
  }
  emit(imageChanged());
}

QSGNode* ImageViewItem::updatePaintNode(QSGNode* _oldNode, UpdatePaintNodeData* _upnd)
{
  Q_UNUSED(_upnd);
  delete m_texture;
  m_texture = window()->createTextureFromImage(m_img);
  QSGSimpleTextureNode* textureNode = new QSGSimpleTextureNode;
  textureNode->setRect(0, 0, width(), height());
  textureNode->setTexture(m_texture);
  delete _oldNode;
  return textureNode;
}
