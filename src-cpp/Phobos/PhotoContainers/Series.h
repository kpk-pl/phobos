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

    ItemPtr best() const;

    std::size_t size() const { return photoItems.size(); }
    QUuid const& uuid() const { return _uuid; }

    ItemPtrVec::const_reference operator[](std::size_t i) const { return photoItems[i]; }
    ItemPtrVec::const_reference item(std::size_t i) const { return (*this)[i]; }
    ItemPtrVec::const_iterator begin() const { return photoItems.begin(); }
    ItemPtrVec::const_iterator end() const { return photoItems.end(); }

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
