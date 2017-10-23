#include "ImportWizard/DivisionOps.h"
#include "Utils/Algorithm.h"
#include "ConfigExtension.h"
#include <easylogging++.h>
#include <deque>

namespace phobos { namespace importwiz {

PhotoSeriesVec divideToSeriesNoop(std::vector<Photo> && photos)
{
  PhotoSeries series;
  series.reserve(photos.size());
  for (auto &p : photos)
    series.append(std::move(p));

  PhotoSeriesVec result;
  result.push_back(series);

  return result;
}

PhotoSeriesVec divideToSeriesWithEqualSize(std::vector<Photo> && photos, std::size_t const photosInSeries)
{
  PhotoSeriesVec result;
  for (std::size_t n = 0; n < photos.size(); ++n)
  {
    if (n % photosInSeries == 0)
    {
      result.push_back(PhotoSeries());
      result.back().reserve(photosInSeries);
    }

    result.back().append(std::move(photos[n]));
  }

  return result;
}

namespace {
double averageTimeDiff(auto beginIt, auto endIt)
{
  unsigned sum = 0;
  unsigned count = 0;
  auto nextIt = beginIt+1;
  while (nextIt != endIt)
  {
    sum += nextIt->info.timestamp - beginIt->info.timestamp;
    ++count;
    ++beginIt;
    ++nextIt;
  }

  return double(sum) / count;
}

bool exifMatches(Photo const& first, Photo const& second)
{
  if (first.info.size != second.info.size)
    return false;

  if (first.info.camera != second.info.camera)
    return false;

  return true;
}

struct DateNameComp
{
  bool operator()(Photo const& first, Photo const& second) const
  {
    if (first.info.timestamp != second.info.timestamp)
      return first.info.timestamp < second.info.timestamp;
    return first.name < second.name;
  }
};
} // unnamed namespace

PhotoSeriesVec divideToSeriesOnMetadata(std::vector<Photo> && photos)
{
  double const timeDrift = config::qualified("photoSet.allowedSecondDriftInSeries", 1.5);

  auto const inRange = [timeDrift](double value, double targetPoint){
    return value <= targetPoint + timeDrift && value >= targetPoint - timeDrift;
  };

  PhotoSeriesVec result;
  std::deque<Photo> stack;

  std::sort(photos.begin(), photos.end(), DateNameComp{});

  // TODO: handle series that have length of 2. probably if exifMatches and timestamp between photos is close
  for (Photo &photo : photos)
  {
    stack.push_back(std::move(photo));
    if (stack.size() < 3)
      continue;

    auto const last = std::next(stack.begin(), stack.size()-1);
    unsigned const lastDiff = last->info.timestamp - (last-1)->info.timestamp;

    if (!inRange(lastDiff, averageTimeDiff(stack.begin(), last)) || !exifMatches(*(last-1), *last))
    {
      /* If only 3 photos on stack and they do not form any series, pop just one photo from the begin */
      auto const endSeries = (stack.size() == 3 ? stack.begin()+1 : last);
      result.push_back(utils::moveFromRange<PhotoSeries>(stack.begin(), endSeries));
      stack.erase(stack.begin(), endSeries);
    }
  }

  if (stack.size() < 3)
    for (auto &el : stack)
      result.push_back(PhotoSeries(1, std::move(el)));
  else
    result.push_back(utils::moveFromRange<PhotoSeries>(stack.begin(), stack.end()));

  return result;
}

}} // namespace phobos::importwiz
