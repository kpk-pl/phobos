#include "LaboratoryView.h"
#include "Widgets/ImageWidget.h"
#include "ImageProcessing/Enhance/Executor.h"
#include "ImageCache/Cache.h"
#include <easylogging++.h>
#include <QHBoxLayout>
#include <QEvent>

namespace phobos {

LaboratoryView::LaboratoryView(pcontainer::Set const& seriesSet, icache::Cache & imageCache) :
  seriesSet(seriesSet), imageCache(imageCache)
{
  imageWidget = new widgets::ImageWidget(QSize(800, 600));

  QHBoxLayout* hlayout = new QHBoxLayout;
  hlayout->setContentsMargins(0, 0, 0, 0);
  hlayout->addWidget(imageWidget, Qt::AlignHCenter | Qt::AlignTop);

  setLayout(hlayout);
}

void LaboratoryView::changePhoto(pcontainer::Item const& item)
{
  currentId = item.id();

  auto const cacheResult = imageCache.transaction().callback([lt=imageWidget->lifetime()](auto && result){
    auto item = lt.lock();
    if (item)
      item->setImage(result.image);
  }).item(item.id()).proactive().execute();

  imageWidget->setImage(cacheResult.image);
  imageWidget->setMaximumSize(item.info().size);
  imageWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

  LOG(TRACE) << "Set up new photo in laboratory: " << item.id().toString();
}

void LaboratoryView::process(iprocess::enhance::OperationType const operation)
{
  using namespace iprocess::enhance;

  LOG(TRACE) << "Laboratory processing operation " << toString(operation);

  QImage result = execute(operation, imageWidget->image());
  imageWidget->setImage(result);
}

} // namespace phobos
