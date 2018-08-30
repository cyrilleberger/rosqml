#include "Image.h"

#include <QDebug>
#include <QImage>

void Image::setImageData(const Image::Data& _imageData)
{
  m_imageData = _imageData;
  emit(imageDataChanged());
}

namespace
{
  constexpr quint8 scale(quint8 _t) { return _t; }
  constexpr quint8 scale(qint8 _t) { return qint16(_t) + 0x7f; }
  constexpr quint8 scale(quint16 _t) { return _t / 0xFF; }
  constexpr quint8 scale(qint16 _t) { return (qint16(_t) + 0x7fff) / 0xFF; }
  constexpr quint8 scale(quint32 _t) { return _t / 0xFFFF; }
  constexpr quint8 scale(qint32 _t) { return (qint32(_t) + 0x7fffffff) / 0xFFFF; }
  constexpr quint8 scale(quint64 _t) { return _t / 0xFFFFFFFF; }
  constexpr quint8 scale(qint64 _t) { return (quint64(_t) - 0x7fffffffffffffff) / 0xFFFFFFFF; }
  constexpr quint8 scale(float _t) { return _t * 0xFF; }
  constexpr quint8 scale(double _t) { return _t * 0xFF; }

  template<typename _T_>
  struct ChannelConverter
  {
    constexpr static quint8 convert(const char* data)
    {
      return scale(*reinterpret_cast<const _T_*>(data));
    }
    constexpr static std::size_t size()
    {
      return sizeof(_T_);
    }
  };
  
  template<int _red_channel_, int _green_channel_, int _blue_channel_, typename _channel_t_>
  struct ToRgb32PixelGenericConverter
  {
    typedef ::ChannelConverter<_channel_t_> ChannelConverter;
    ToRgb32PixelGenericConverter()
    {
      const quint32 size = ChannelConverter::size();
      red_offset    = _red_channel_ * size;
      green_offset  = _green_channel_ * size;
      blue_offset   = _blue_channel_ * size;
    }
    constexpr void convert(const char* _indata, quint8* _outdata)
    {
      _outdata[0] = ChannelConverter::convert(_indata + red_offset);
      _outdata[1] = ChannelConverter::convert(_indata + green_offset);
      _outdata[2] = ChannelConverter::convert(_indata + blue_offset);
      _outdata[3] = 0xff; // alpha
    }
    constexpr QImage::Format format()
    {
      return QImage::Format_RGB32;
    }
    constexpr std::size_t imgPixelSize()
    {
      return 4;
    }
    constexpr std::size_t dataPixelSize()
    {
      return 3 * ChannelConverter::size();
    }
    quint32 red_offset, green_offset, blue_offset;
  };
  
  template<typename _channel_t_>
  using RgbToRgb32PixelConverter = ToRgb32PixelGenericConverter<2, 1, 0, _channel_t_>;
  template<typename _channel_t_>
  using BgrToRgb32PixelConverter = ToRgb32PixelGenericConverter<0, 1, 2, _channel_t_>;
  
  template<template<typename> class _PixelConverter_>
  struct Converter
  {
    static QImage convert(const Image::Data& _source)
    {
      switch(_source.depth)
      {
        case Image::Depth::UInt8:
          return do_convert<quint8>(_source);
          break;
        case Image::Depth::Int8:
          return do_convert<qint8>(_source);
          break;
        case Image::Depth::UInt16:
          return do_convert<quint16>(_source);
          break;
        case Image::Depth::Int16:
          return do_convert<qint16>(_source);
          break;
        case Image::Depth::UInt32:
          return do_convert<quint32>(_source);
          break;
        case Image::Depth::Int32:
          return do_convert<qint32>(_source);
          break;
        case Image::Depth::UInt64:
          return do_convert<quint64>(_source);
          break;
        case Image::Depth::Int64:
          return do_convert<qint64>(_source);
          break;
        case Image::Depth::Float32:
          return do_convert<float>(_source);
          break;
        case Image::Depth::Float64:
          return do_convert<double>(_source);
          break;
      }
      qFatal("Unsupported depth");
    }
  private:
    template<typename _T_>
    static QImage do_convert(const Image::Data& _source)
    {
      _PixelConverter_<_T_> pc;
      QImage img(_source.width, _source.height, pc.format());
      
      int pos_img = 0;
      int pos_data = 0;
      
      std::size_t data_stride = pc.dataPixelSize();
      std::size_t img_stride = pc.imgPixelSize();
      
      for(std::size_t i = 0; i < _source.width * _source.height; ++i)
      {
        pc.convert(_source.data.data() + pos_data, img.bits() + pos_img);
        pos_data += data_stride;
        pos_img  += img_stride;
        
      }
      return img;
    }
  };
}

QImage Image::toQImage() const
{
  if(m_imageData.isEmpty()) return QImage();
  
  switch(m_imageData.colorspace)
  {
    case Image::Colorspace::RGB:
      return Converter<RgbToRgb32PixelConverter>::convert(m_imageData);
    case Image::Colorspace::BGR:
      return Converter<BgrToRgb32PixelConverter>::convert(m_imageData);
    default:
      qWarning() << "Unsupported colorspace: " << int(m_imageData.colorspace);
      return QImage();
  }
}
