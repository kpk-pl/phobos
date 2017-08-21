#include "PhotoContainers/ExifData.h"
#include "qt_ext/qexifimageheader.h"
#include <QImage>

namespace phobos { namespace pcontainer {

ExifData::ExifData(QString const& fileName)
{
  QExifImageHeader const exif(fileName);

  using ImageTag = QExifImageHeader::ImageTag;
  using ExtendedTag = QExifImageHeader::ExifExtendedTag;

  if (exif.contains(ImageTag::ImageWidth) && exif.contains(ImageTag::ImageLength))
  {
    width = exif.value(ImageTag::ImageWidth).toLong();
    height = exif.value(ImageTag::ImageLength).toLong();
  }
  else if (exif.contains(ExtendedTag::PixelXDimension) &&
           exif.contains(ExtendedTag::PixelYDimension))
  {
    width = exif.value(ExtendedTag::PixelXDimension).toLong();
    height = exif.value(ExtendedTag::PixelYDimension).toLong();
  }
}

}} // namespace phobos::pcontainer
