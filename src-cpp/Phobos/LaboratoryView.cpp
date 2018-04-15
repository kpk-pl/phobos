#include "LaboratoryView.h"
#include "Widgets/ImageWidget.h"
#include "ImageProcessing/Enhance/Executor.h"
#include "ImageCache/Cache.h"
#include <easylogging++.h>
#include <QHBoxLayout>
#include <QProgressDialog>
#include <QThread>

namespace phobos {

LaboratoryView::LaboratoryView(pcontainer::Set const& seriesSet, icache::Cache & imageCache) :
  seriesSet(seriesSet), imageCache(imageCache)
{
  imageWidget = new widgets::ImageWidget(QSize(800, 600));
  imageWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

  QHBoxLayout* hlayout = new QHBoxLayout;
  hlayout->setContentsMargins(0, 0, 0, 0);
  hlayout->addWidget(imageWidget, Qt::AlignHCenter | Qt::AlignTop);

  setLayout(hlayout);
}

void LaboratoryView::changePhoto(pcontainer::Item const& item)
{
  currentId = item.id();
  resetImage();

  imageWidget->setMaximumSize(item.info().size);

  LOG(TRACE) << "Set up new photo in laboratory: " << item.id().toString();
}

void LaboratoryView::resetImage()
{
  auto const cacheResult = imageCache.transaction().callback([lt=imageWidget->lifetime(), id = *currentId, this](auto && result){
    auto item = lt.lock();
    if (item && currentId == id)
      item->setImage(result.image);
  }).item(*currentId).proactive().execute();

  LOG(TRACE) << "Reseting photo in laboratory";
  imageWidget->setImage(cacheResult.image);

  LOG(TRACE) << "Clearing laboratory operation stack";
  operationStack.clear();
}

void LaboratoryView::process(iprocess::enhance::OperationType const operation)
{
  using namespace iprocess::enhance;

  LOG(TRACE) << "Laboratory processing operation " << toString(operation);

  QImage result = Executor::processOne(imageWidget->image(), operation);
  imageWidget->setImage(result);
  operationStack.push_back(operation);
}

void LaboratoryView::saveItem(QString const fileName)
{
  if (!currentId)
    return;

  LOG(TRACE) << "Saving processed item " << currentId->toString() << " to " << fileName;

  using namespace iprocess::enhance;
  Executor* job = new Executor(*currentId, operationStack, fileName);

  QProgressDialog *progress = new QProgressDialog(this);
  progress->setWindowTitle(tr("Processing..."));

  job->runInThread(new QThread(this), progress);
}

} // namespace phobos
