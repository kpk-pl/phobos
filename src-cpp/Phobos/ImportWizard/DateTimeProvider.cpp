#include "ImportWizard/DateTimeProvider.h"
#include "Utils/Filesystem/Attributes.h"
#include <qt_ext/qexifimageheader.h>
#include <easylogging++.h>
#include <QDateTime>
#include <thread>
#include <future>

namespace phobos { namespace importwiz {

namespace {
void paralellThransformModTime(std::vector<Photo>::iterator destination,
                               QStringList::const_iterator const& begin,
                               QStringList::const_iterator const& end)
{
  // TODO: BUG Currect Exif library has some sort of a bug that makes reading data fields
  // read only 19 bytes instead of 20 and thus making datetime invalid
  // switch to https://github.com/mayanklahiri/easyexif lib which was tested and does not have this bug
  std::transform(begin, end, destination, [](QString const& str) -> Photo {
    QExifImageHeader const header(str);

    if (header.contains(QExifImageHeader::ExifExtendedTag::DateTimeOriginal))
    {
      auto const v = header.value(QExifImageHeader::ExifExtendedTag::DateTimeOriginal).toDateTime();
      if (v.isValid()) return {str, v.toSecsSinceEpoch()};
    }
    if (header.contains(QExifImageHeader::ImageTag::DateTime))
    {
      auto const v = header.value(QExifImageHeader::ImageTag::DateTime).toDateTime();
      if (v.isValid()) return {str, v.toSecsSinceEpoch()};
    }
    return {str, utils::fs::lastModificationTime(str.toStdString())};
  });
}

} // unnamed namespace

std::vector<Photo> provideDateTime(QStringList const& photos)
{
  TIMED_SCOPE(scopeFunc, "provideDateTime");

  std::vector<Photo> result;
  result.resize(photos.size());

  unsigned const numThreads = std::max(1u, std::thread::hardware_concurrency());
  std::vector<std::future<void>> futures;
  futures.reserve(numThreads-1);

  auto destIt = result.begin();
  auto sourceIt = photos.begin();

  if (photos.size() > 100)
  {
    std::size_t const increment = photos.size()/numThreads;
    for (unsigned i = 0; i < numThreads-1; ++i)
    {
      futures.emplace_back(std::async(std::launch::async, paralellThransformModTime, destIt, sourceIt, std::next(sourceIt, increment)));
      std::advance(destIt, increment);
      std::advance(sourceIt, increment);
    }
  }

  paralellThransformModTime(destIt, sourceIt, photos.end());

  for (auto const& f : futures)
    f.wait();

  return result;
}

}} // namespace phobos::importwiz
