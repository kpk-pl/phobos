#ifndef LABORATORYVIEW_H
#define LABORATORYVIEW_H

#include "PhotoContainers/Item.h"
#include "ImageCache/CacheFwd.h"
#include "ImageProcessing/Enhance/OperationType.h"
#include <QWidget>
#include <vector>

namespace phobos {

namespace widgets {
class ImageWidget;
} // namespace widgets

// TODO: common class with seriesSet and cache there in protected

class LaboratoryView : public QWidget
{
Q_OBJECT
public:
  explicit LaboratoryView(pcontainer::Set const& seriesSet, icache::Cache & imageCache);

  void changePhoto(pcontainer::Item const& item);
  boost::optional<pcontainer::ItemId> const& currentItem() const { return currentId; }

public slots:
  void process(iprocess::enhance::OperationType const operation);
  void resetImage();
  void saveItem(QString const fileName);

private:
  pcontainer::Set const& seriesSet;
  icache::Cache & imageCache;

  widgets::ImageWidget *imageWidget;

  boost::optional<pcontainer::ItemId> currentId;
  std::vector<iprocess::enhance::OperationType> operationStack;
};

} // namespace phobos

#endif // LABORATORYVIEW_H
