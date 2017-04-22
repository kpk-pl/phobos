#include <easylogging++.h>
#include "ImportWizard/DivisionOps.h"
#include "Utils/FileAttributes.h"
#include "ConfigExtension.h"

namespace phobos { namespace importwiz {

std::vector<PhotoSeries> divideToSeriesWithEqualSize(QStringList const& photos, std::size_t const photosInSeries)
{
    assert(photosInSeries > 0);

    std::vector<PhotoSeries> result;
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

std::vector<PhotoSeries> divideToSeriesOnMetadata(QStringList const& photos)
{
    std::vector<Photo> photosWithTime;
    photosWithTime.reserve(photos.size());

    std::transform(photos.begin(), photos.end(), std::back_inserter(photosWithTime),
            [](QString const& str){
                return Photo{str.toStdString(), utils::lastModificationTime(str.toStdString())};
            });

    std::stable_sort(photosWithTime.begin(), photosWithTime.end(),
            [](Photo const& fc1, Photo const& fc2) {
                return *fc1.lastModTime < *fc2.lastModTime;
            });

    std::vector<PhotoSeries> result;
    PhotoSeries current;
    unsigned const timeThreshold = config::qualified("photoSet.seriesTimeThreshold", 2u);

    for (Photo const& photo : photosWithTime)
    {
        if (!current.empty() && (*photo.lastModTime - *current.back().lastModTime > timeThreshold))
        {
            result.emplace_back(std::move(current));
            current.clear();
        }
        current.push_back(photo);
    }

    if (!current.empty())
        result.emplace_back(std::move(current));

    return result;
}

}} // namespace phobos::importwiz
