#include "ImportWizard/DivisionOps.h"
#include "Utils/Algorithm.h"
#include "ConfigExtension.h"
#include "ConfigPath.h"
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
  config::ConfigPath const configPath("photoSet");
  double const timeDrift = config::qualified(configPath("allowedSecondDriftInSeries"), 1.5);
  double const maxTimeBtwTwo = config::qualified(configPath("maximumTimeBetweenTwoPhotosInSeries"), 2.0);

  auto const inRange = [timeDrift](double value, double targetPoint){
    return value <= targetPoint + timeDrift && value >= targetPoint - timeDrift;
  };

  PhotoSeriesVec result;
  std::deque<Photo> stack;

  auto const makeSeries = [&result, &stack](auto&& end){
    result.push_back(utils::moveFromRange<PhotoSeries>(stack.begin(), end));
    stack.erase(stack.begin(), end);
  };

  std::sort(photos.begin(), photos.end(), DateNameComp{});

  for (Photo &photo : photos)
  {
    stack.push_back(std::move(photo));
    if (stack.size() < 2)
      continue;

    auto const butLast = std::next(stack.begin(), stack.size()-2);
    auto const last = std::next(butLast);
    if (!exifMatches(stack.front(), stack.back()))
    {
      // If newly added photo does not exif match to the rest of photos, then immediatelly divide series
      makeSeries(last);
      continue;
    }

    if (stack.size() < 3)
      continue;

    unsigned const lastDiff = last->info.timestamp - butLast->info.timestamp;

    if (!inRange(lastDiff, averageTimeDiff(stack.begin(), last)))
    {
      // If only 3 photos on stack and they do not form any series, pop just one photo from the begin
      if (stack.size() == 3)
      {
        unsigned const firstDiff = butLast->info.timestamp - stack.front().info.timestamp;
        if (firstDiff > maxTimeBtwTwo)
        {
          makeSeries(std::next(stack.begin()));
          continue;
        }
      }
      // fallthrough
      // else make series from all but last photo
      makeSeries(last);
    }
  }

  if (stack.size() == 2)
  {
    unsigned const diff = stack.back().info.timestamp - stack.front().info.timestamp;
    if (diff > maxTimeBtwTwo)
    {
      makeSeries(std::next(stack.begin()));
      makeSeries(stack.end());
    }
  }

  if (!stack.empty())
    makeSeries(stack.end());

  return result;
}

}} // namespace phobos::importwiz
