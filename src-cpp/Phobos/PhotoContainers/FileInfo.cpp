#include "PhotoContainers/FileInfo.h"
#include "Utils/ExifReader.h"
#include "Utils/Filesystem/Attributes.h"
#include <QImage>
#include <QImageReader>

namespace phobos { namespace pcontainer {

namespace {
QSize getImageSize(QString const& fileName, boost::optional<easyexif::EXIFInfo> const& exif)
{
  if (exif)
  {
    QSize result(exif->ImageWidth, exif->ImageHeight);
    if (result.width() && result.height())
      return result;
  }

  return QImageReader{fileName}.size();
}

unsigned getImageDatetime(QString const& fileName, boost::optional<easyexif::EXIFInfo> const& exif)
{
  static auto const fmt = QLatin1String("yyyy:MM:dd HH:mm:ss");

  if (exif)
  {
    if (exif->DateTimeOriginal.length() == static_cast<unsigned>(fmt.size()))
      return QDateTime::fromString(QString::fromStdString(exif->DateTimeOriginal), fmt).toSecsSinceEpoch();

    if (exif->DateTime.length() == static_cast<unsigned>(fmt.size()))
      return QDateTime::fromString(QString::fromStdString(exif->DateTime), fmt).toSecsSinceEpoch();
  }

  return utils::fs::lastModificationTime(fileName.toStdString());
}
} // unnamed namespace

FileInfo::FileInfo(QString const& fileName)
{
  auto const exifHeader = utils::readExif(fileName.toStdString());

  timestamp = getImageDatetime(fileName, exifHeader);
  size = getImageSize(fileName, exifHeader);

  if (exifHeader)
  {
    camera = CameraInfo{};
    camera->make = QString::fromStdString(exifHeader->Make);
    camera->model = QString::fromStdString(exifHeader->Model);

    shot = ShotInfo{};
    shot->exposureTime = exifHeader->ExposureTime;
    shot->exposureBias = exifHeader->ExposureBiasValue;
    shot->fnumber = exifHeader->FNumber;
    shot->ISOSpeed = exifHeader->ISOSpeedRatings;
    shot->focalLen = exifHeader->FocalLength;
    shot->flash = static_cast<bool>(exifHeader->Flash);
  }
}

bool FileInfo::operator==(FileInfo const& other) const
{
  return timestamp == other.timestamp && size == other.size;
}

bool FileInfo::CameraInfo::operator==(CameraInfo const& other) const
{
  return make == other.make && model == other.model;
}

}} // namespace phobos::pcontainer
