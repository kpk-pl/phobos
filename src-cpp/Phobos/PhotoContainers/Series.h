#ifndef PHOTO_CONTAINERS_SERIES_H
#define PHOTO_CONTAINERS_SERIES_H

#include <vector>
#include <QUuid>
#include <QObject>
#include "PhotoContainers/Item.h"
#include "ImageProcessing/Metrics.h"

namespace phobos { namespace pcontainer {

class Series : public QObject
{
    Q_OBJECT
public:
    explicit Series();

    void addPhotoItems(std::vector<std::string> const& fileNames);
    void addPhotoItem(std::string const& fileName);

    std::size_t size() const { return photoItems.size(); }
    QUuid const& uuid() const { return _uuid; }

    ItemPtr const& item(std::size_t const& n) const { return photoItems[n]; }
    ItemPtrVec const& items() const { return photoItems; }

private slots:
    void newMetricCalculated();

private:   
    ItemPtrVec photoItems;
    QUuid const _uuid;
};

using SeriesPtr = std::shared_ptr<Series>;
using SeriesPtrVec = std::vector<SeriesPtr>;

}} // namespace phobos::pcontainer

#endif // PHOTO_CONTAINERS_SERIES_H
