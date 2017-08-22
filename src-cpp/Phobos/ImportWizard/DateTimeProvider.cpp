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
  std::transform(begin, end, destination, [](QString const& str) {
    QExifImageHeader const header(str);
    auto const dateTime = header.contains(QExifImageHeader::ImageTag::DateTime)
        ? header.value(QExifImageHeader::ImageTag::DateTime).toDateTime().toSecsSinceEpoch()
        : utils::fs::lastModificationTime(str.toStdString());

    return Photo{str, dateTime};
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
