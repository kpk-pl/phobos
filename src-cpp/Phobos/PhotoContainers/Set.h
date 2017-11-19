#ifndef PHOTO_CONTAINERS_SET_H
#define PHOTO_CONTAINERS_SET_H

#include <QObject>
#include <QUuid>
#include "PhotoContainers/Fwd.h"
#include "PhotoContainers/Series.h"
#include "ImportWizard/Types.h"

namespace phobos { namespace pcontainer {

class Set : public QObject
{
  Q_OBJECT

public:
  explicit Set() = default;

  void addSeries(importwiz::PhotoSeriesVec const& newPhotoSeries);

  Series const& findSeries(QUuid const& seriesUuid, int const offset = 0) const;
  Series const& findNonEmptySeries(QUuid const& seriesUuid, int const offset = 0) const;

  ItemPtr findItem(ItemId const& id) const;

  std::size_t size() const { return _photoSeries.size(); }
  bool empty() const { return _photoSeries.empty(); }
  bool hasPhotos() const;
  Series const& front() const;

  SeriesPtrVec::const_iterator begin() const { return _photoSeries.begin(); }
  SeriesPtrVec::const_iterator end() const { return _photoSeries.end(); }

signals:
  void newSeries(SeriesPtr);
  void changedSeries(QUuid seriesUuid);

public slots:
  void removeImage(pcontainer::ItemId const& itemId);
  void removeImages(std::vector<pcontainer::ItemId> itemIds);
  void removeSeries(QUuid const seriesUuid);

private:
  SeriesPtr const& findSeriesImpl(QUuid const& seriesUuid, int const offset = 0) const;
  void removeImagesImpl(std::vector<pcontainer::ItemId> const& itemIds);

  SeriesPtrVec _photoSeries;
};

}} // namespace phobos::pcontainer

#endif // PHOTO_CONTAINERS_SET_H
