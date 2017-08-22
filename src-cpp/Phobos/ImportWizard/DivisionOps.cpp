#include "ImportWizard/DivisionOps.h"
#include "Utils/Comparators.h"
#include "Utils/Algorithm.h"
#include "ConfigExtension.h"
#include <easylogging++.h>
#include <deque>

namespace phobos { namespace importwiz {

PhotoSeriesVec divideToSeriesNoop(std::vector<Photo> const& photos)
{
  PhotoSeries series;
  series.reserve(photos.size());
  for (auto const& p : photos)
    series.append(p);

  PhotoSeriesVec result;
  result.push_back(series);

  return result;
}

PhotoSeriesVec divideToSeriesWithEqualSize(std::vector<Photo> const& photos, std::size_t const photosInSeries)
{
  PhotoSeriesVec result;
  for (std::size_t n = 0; n < photos.size(); ++n)
  {
    if (n % photosInSeries == 0)
    {
      result.push_back(PhotoSeries());
      result.back().reserve(photosInSeries);
    }

    result.back().append(photos[n]);
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
    sum += *nextIt->lastModTime - *beginIt->lastModTime;
    ++count;
    ++beginIt;
    ++nextIt;
  }

  return double(sum) / count;
}
} // unnamed namespace

PhotoSeriesVec divideToSeriesOnMetadata(std::vector<Photo> const& photos)
{
    std::vector<Photo> photosWithTime = photos;
    std::stable_sort(photosWithTime.begin(), photosWithTime.end(), utils::less().on([](Photo const& p){ return *p.lastModTime; }));

    double const timeDrift = config::qualified("photoSet.allowedSecondDriftInSeries", 1.5);

    auto const inRange = [timeDrift](double value, double targetPoint){
        return value <= targetPoint + timeDrift && value >= targetPoint - timeDrift;
    };

    PhotoSeriesVec result;
    std::deque<Photo> stack;

    for (Photo const& photo : photosWithTime)
    {
        stack.push_back(photo);
        if (stack.size() < 3)
            continue;

        auto const last = stack.begin() + (stack.size()-1);
        unsigned const lastDiff = *last->lastModTime - *(last-1)->lastModTime;

        if (!inRange(lastDiff, averageTimeDiff(stack.begin(), last)))
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
