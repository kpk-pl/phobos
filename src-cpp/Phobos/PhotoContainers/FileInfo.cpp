#include "PhotoContainers/FileInfo.h"
#include "Utils/Filesystem/Attributes.h"
#include <qt_ext/qexifimageheader.h>
#include <QImage>
#include <QImageReader>

namespace phobos { namespace pcontainer {

namespace {
QSize getImageSize(QString const& fileName, QExifImageHeader const& exif)
{
  using Tag = QExifImageHeader::ImageTag;

  if (exif.contains(Tag::ImageWidth) && exif.contains(Tag::ImageLength))
  {
    QSize result(exif.value(Tag::ImageWidth).toLong(), exif.value(Tag::ImageLength).toLong());
    if (result.width() && result.height())
      return result;
  }

  return QImageReader{fileName}.size();
}

unsigned getImageDatetime(QString const& fileName, QExifImageHeader const& exif)
{
  using Tag = QExifImageHeader::ImageTag;
  using XTag = QExifImageHeader::ExifExtendedTag;

  if (exif.contains(XTag::DateTimeOriginal))
  {
    auto const dt = exif.value(XTag::DateTimeOriginal).toDateTime();
    if (dt.isValid())
      return dt.toSecsSinceEpoch();
  }

  if (exif.contains(Tag::DateTime))
  {
    auto const dt = exif.value(Tag::DateTime).toDateTime();
    if (dt.isValid())
      return dt.toSecsSinceEpoch();
  }

  return utils::fs::lastModificationTime(fileName.toStdString());
}
} // unnamed namespace

FileInfo::FileInfo(QString const& fileName)
{
  QExifImageHeader exif(fileName);

  timestamp = getImageDatetime(fileName, exif);
  size = getImageSize(fileName, exif);

  using Tag = QExifImageHeader::ImageTag;
  using XTag = QExifImageHeader::ExifExtendedTag;

  if (exif.contains(Tag::Make))
    camera.make = exif.value(Tag::Make).toString();
  if (exif.contains(Tag::Model))
    camera.model = exif.value(Tag::Model).toString();

  if (exif.contains(Tag::Orientation))
    shot.orientation = exif.value(Tag::Orientation).toShort();
  if (exif.contains(XTag::ExposureTime))
  {
    auto const val = exif.value(XTag::ExposureTime).toRational();
    shot.exposureTime = std::make_pair(val.first, val.second);
  }
  if (exif.contains(XTag::ExposureBiasValue))
  {
    auto const val = exif.value(XTag::ExposureBiasValue).toSignedRational();
    shot.exposureBias = double(val.first) / double(val.second);
  }
  if (exif.contains(XTag::FNumber))
  {
    auto const val = exif.value(XTag::FNumber).toRational();
    shot.fnumber = std::make_pair(val.first, val.second);
  }
  if (exif.contains(XTag::ISOSpeedRatings))
    shot.ISOSpeed = exif.value(XTag::ISOSpeedRatings).toShort();
  if (exif.contains(XTag::FocalLength))
  {
    auto const val = exif.value(XTag::FocalLength).toRational();
    shot.focalLen = double(val.first) / double(val.second);
  }
  if (exif.contains(XTag::Flash))
    shot.flash = exif.value(XTag::Flash).toShort() & 0x01;
}

bool FileInfo::operator==(FileInfo const& other) const
{
  return timestamp == other.timestamp && size == other.size;
}

bool FileInfo::CameraInfo::operator==(CameraInfo const& other) const
{
  return make == other.make && model == other.model;
}

bool FileInfo::CameraInfo::operator!=(CameraInfo const& other) const
{
  return !(*this == other);
}

}} // namespace phobos::pcontainer
