#ifndef LABORATORYVIEW_H
#define LABORATORYVIEW_H

#include "PhotoContainers/Item.h"
#include "Views/View.h"
#include "ImageProcessing/Enhance/OperationType.h"
#include <vector>

namespace phobos {

namespace widgets {
class ImageWidget;
} // namespace widgets

namespace view {

class Laboratory : public View
{
Q_OBJECT
public:
  explicit Laboratory(pcontainer::Set const& seriesSet, icache::Cache & imageCache);

  void changePhoto(pcontainer::Item const& item);
  boost::optional<pcontainer::ItemId> const& currentItem() const { return currentId; }

public slots:
  void process(iprocess::enhance::OperationType const operation);
  void resetImage();
  void saveItem(QString const fileName);

private:
  widgets::ImageWidget *imageWidget;

  boost::optional<pcontainer::ItemId> currentId;
  std::vector<iprocess::enhance::OperationType> operationStack;
};

}} // namespace phobos::view

#endif // LABORATORYVIEW_H
