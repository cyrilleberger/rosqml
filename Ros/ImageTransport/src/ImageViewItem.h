#ifndef _IMAGEVIEWITEM_H_
#define _IMAGEVIEWITEM_H_

#include <QQuickItem>
#include <QImage>
#include <QThreadPool>

class Image;
class QSGTexture;

class ImageViewItem : public QQuickItem
{
  Q_OBJECT
  Q_PROPERTY(Image* image READ image WRITE setImage NOTIFY imageChanged)
public:
  ImageViewItem(QQuickItem* _parent = 0);
  virtual ~ImageViewItem();
public:
  Image* image() const;
  void setImage(Image* _image);
signals:
  void imageChanged();
protected:
  virtual QSGNode* updatePaintNode(QSGNode* _oldNode, UpdatePaintNodeData* _upnd);
private:
  Image* m_image;
  QImage m_img;
  QSGTexture* m_texture = nullptr;
  QThreadPool m_pool;
};

#endif
