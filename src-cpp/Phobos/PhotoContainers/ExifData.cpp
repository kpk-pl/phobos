#include "PhotoContainers/ExifData.h"
#include "qt_ext/qexifimageheader.h"
#include <QImage>
#include <QImageReader>

namespace phobos { namespace pcontainer {

namespace {
QSize getImageSize(QString const& fileName)
{
  QExifImageHeader const exif(fileName);

  using ImageTag = QExifImageHeader::ImageTag;
  using ExtendedTag = QExifImageHeader::ExifExtendedTag;

  if (exif.contains(ImageTag::ImageWidth) && exif.contains(ImageTag::ImageLength))
    return QSize(exif.value(ImageTag::ImageWidth).toLong(), exif.value(ImageTag::ImageLength).toLong());
  else if (exif.contains(ExtendedTag::PixelXDimension) && exif.contains(ExtendedTag::PixelYDimension))
    return QSize(exif.value(ExtendedTag::PixelXDimension).toLong(), exif.value(ExtendedTag::PixelYDimension).toLong());
  else
    return QImageReader{fileName}.size();
}

QDateTime getImageTimestamp(boost::optional<unsigned> secsSinceEpoch)
{
  if (!secsSinceEpoch)
    return QDateTime{};
  return QDateTime::fromSecsSinceEpoch(*secsSinceEpoch);
}
} // unnamed namespace

ExifData::ExifData(importwiz::Photo const& photoDesc) :
  size(getImageSize(photoDesc.fileName)),
  timestamp(getImageTimestamp(photoDesc.lastModTime))
{}

}} // namespace phobos::pcontainer
