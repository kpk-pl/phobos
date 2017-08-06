#ifndef PHOTO_CONTAINERS_SERIES_H
#define PHOTO_CONTAINERS_SERIES_H

#include <QUuid>
#include <QObject>
#include "PhotoContainers/Fwd.h"
#include "PhotoContainers/Item.h"
#include "ImportWizard/Types.h"

namespace phobos { namespace pcontainer {

class Series : public QObject
{
    Q_OBJECT
public:
    explicit Series();
    explicit Series(importwiz::PhotoSeries const& series);

    void addPhotoItems(std::vector<std::string> const& fileNames);
    void addPhotoItem(std::string const& fileName);

    std::size_t size() const { return photoItems.size(); }
    bool empty() const { return photoItems.empty(); }
    QUuid const& uuid() const { return _uuid; }

    ItemPtrVec::const_reference operator[](std::size_t i) const { return photoItems[i]; }
    ItemPtrVec::const_reference item(std::size_t i) const { return (*this)[i]; }
    ItemPtrVec::const_iterator begin() const { return photoItems.begin(); }
    ItemPtrVec::const_iterator end() const { return photoItems.end(); }

    void remove(QString const& fileName);

private:
    ItemPtrVec photoItems;
    QUuid const _uuid;
};



}} // namespace phobos::pcontainer

#endif // PHOTO_CONTAINERS_SERIES_H
