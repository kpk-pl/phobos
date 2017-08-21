#include "ImportWizard/DivisionOps.h"
#include "Utils/Filesystem/Attributes.h"
#include "Utils/Comparators.h"
#include "Utils/Algorithm.h"
#include "ConfigExtension.h"
#include <qt_ext/qexifimageheader.h>
#include <easylogging++.h>
#include <QDateTime>
#include <thread>
#include <future>
#include <deque>

namespace phobos { namespace importwiz {

PhotoSeriesVec divideToSeriesNoop(QStringList const& photos)
{
    PhotoSeriesVec result;
    result.push_back(PhotoSeries());

    result.back().reserve(photos.size());
    for (auto const& photo : photos)
        result.back().push_back(Photo{photo, boost::none});
    return result;
}

PhotoSeriesVec divideToSeriesWithEqualSize(QStringList const& photos, std::size_t const photosInSeries)
{
    PhotoSeriesVec result;
    for (int n = 0; n < photos.size(); ++n)
    {
        if (n % photosInSeries == 0)
        {
            result.push_back(PhotoSeries());
            result.back().reserve(photosInSeries);
        }
        result.back().push_back(Photo{photos[n], boost::none});
    }

    return result;
}

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

    std::vector<Photo> processPhotosForModTime(QStringList const& photos)
    {
        TIMED_SCOPE(scopeFunc, "processPhotosForModTime");

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

PhotoSeriesVec divideToSeriesOnMetadata(QStringList const& photos)
{
    std::vector<Photo> photosWithTime = processPhotosForModTime(photos);
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
