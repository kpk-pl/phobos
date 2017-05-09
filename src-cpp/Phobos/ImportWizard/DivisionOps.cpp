#include <easylogging++.h>
#include "ImportWizard/DivisionOps.h"
#include "Utils/FileAttributes.h"
#include "Utils/Comparators.h"
#include "ConfigExtension.h"
#include <fstream>
#include <thread>
#include <future>

namespace phobos { namespace importwiz {

PhotoSeriesVec divideToSeriesWithEqualSize(QStringList const& photos, std::size_t const photosInSeries)
{
    assert(photosInSeries > 0);

    PhotoSeriesVec result;
    for (int n = 0; n < photos.size(); ++n)
    {
        if (n % photosInSeries == 0)
        {
            result.push_back(PhotoSeries());
            result.back().reserve(photosInSeries);
        }
        result.back().push_back(Photo{photos[n].toStdString(), boost::none});
    }

    return result;
}

namespace {
    void paralell_process(std::vector<Photo>::iterator destination,
                          QStringList::const_iterator const& begin,
                          QStringList::const_iterator const& end)
    {
        std::transform(begin, end, destination, [](QString const& str){
            return Photo{str.toStdString(), utils::lastModificationTime(str.toStdString())};
        });
    }

    std::vector<Photo> processPhotos(QStringList const& photos)
    {
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
                futures.emplace_back(std::async(std::launch::async, paralell_process, destIt, sourceIt, std::next(sourceIt, increment)));
                std::advance(destIt, increment);
                std::advance(sourceIt, increment);
            }
        }

        paralell_process(destIt, sourceIt, photos.end());

        for (auto const& f : futures)
            f.wait();

        return result;
    }
} // unnamed namespace

PhotoSeriesVec divideToSeriesOnMetadata(QStringList const& photos)
{
    // TODO: use EXIF creation time when available
    // TODO: detect brakes automatically, inteligently

    std::vector<Photo> photosWithTime = processPhotos(photos);
    std::stable_sort(photosWithTime.begin(), photosWithTime.end(), utils::less().on([](Photo const& p){ return *p.lastModTime; }));

    PhotoSeriesVec result;
    PhotoSeries current;
    unsigned const timeThreshold = config::qualified("photoSet.seriesTimeThreshold", 2u);

    for (Photo const& photo : photosWithTime)
    {
        if (!current.empty() && (*photo.lastModTime - *current.back().lastModTime > timeThreshold))
        {
            result.push_back(current);
            current.clear();
        }
        current.push_back(photo);
    }

    if (!current.empty())
        result.push_back(current);

    return result;
}

}} // namespace phobos::importwiz
