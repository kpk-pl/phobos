#include "PhotoContainers/Series.h"
//#include "ImageProcessing/MetricsAggregate.h"
//#include "ImageProcessing/MetricsIO.h"
//#include "ConfigExtension.h"

namespace phobos { namespace pcontainer {

Series::Series() :
    _uuid(QUuid::createUuid())
{
}

Series::Series(importwiz::PhotoSeries const& series) :
    Series()
{
    for (auto const& item : series)
        addPhotoItem(item.fileName);
}

void Series::addPhotoItems(std::vector<std::string> const& fileNames)
{
    photoItems.reserve(photoItems.size() + fileNames.size());
    for (auto const& fn : fileNames)
        addPhotoItem(fn);
}

void Series::addPhotoItem(std::string const& fileName)
{
    auto newItem = std::make_shared<Item>(fileName, _uuid, photoItems.size());
    photoItems.emplace_back(std::move(newItem));
}

//ItemPtr Series::best() const
//{
    //for (auto const& item : photoItems)
        //if (item->scoredMetric() && item->scoredMetric()->bestQuality)
            //return item;

    //return nullptr;
//}

}} // namespace phobos::pcontainer
