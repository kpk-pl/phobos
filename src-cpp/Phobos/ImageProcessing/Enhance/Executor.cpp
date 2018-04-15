#include "ImageProcessing/Enhance/Executor.h"
#include "ImageProcessing/Enhance/AutoWhiteBalance.h"
#include "ImageProcessing/Utils/FormatConversion.h"
#include "Utils/Asserted.h"
#include "ConfigPath.h"
#include "ConfigExtension.h"
#include <easylogging++.h>
#include <QImageReader>
#include <QProgressDialog>
#include <QThread>

namespace phobos { namespace iprocess { namespace enhance {

Executor::Executor(pcontainer::ItemId const& itemId,
                   std::vector<iprocess::enhance::OperationType> operations,
                   QString const outputName) :
  itemId(itemId),
  operations(std::move(operations)),
  outputName(outputName)
{}

void Executor::run()
{
  emit progress(0);
  LOG(TRACE) << "Processing save for " << itemId.toString();

  if (operations.empty())
  {
    LOG(TRACE) << "No operations to perform";
    return;
  }

  QImage fullImage = readImage();
  if (fullImage.isNull())
  {
    LOG(TRACE) << "Cannot read image from disc";
    return;
  }

  if (checkCancel())
    return;

  for (std::size_t i = 0; i < operations.size(); ++i)
  {
    emit info(QObject::tr("Processing %1 (%2/%3)").arg(QString::fromStdString(toString(operations[i]))).arg(i).arg(operations.size()));
    fullImage = processOne(fullImage, operations[i]);

    emit progress(100.0 * (2 + i) / (2 + operations.size()));
    LOG(TRACE) << "Operation finished (" << toString(operations[i]) << ")";

    if (checkCancel())
      return;
  }

  saveImage(fullImage);
  emit finished();
}

QImage Executor::readImage()
{
  emit info(QObject::tr("Loading image"));

  QImageReader reader(itemId.fileName);
  reader.setAutoTransform(true);
  reader.setAutoDetectImageFormat(true);

  QImage fullImage = reader.read();
  if (fullImage.isNull())
    return QImage{};

  emit progress(100.0 * 1 / (2 + operations.size()));
  LOG(TRACE) << "Successfully read image from disc";

  return fullImage;
}

void Executor::saveImage(QImage const& image)
{
  emit info(QObject::tr("Saving image"));

  if (!config::qualified(config::ConfigPath("debug")("disableEnhancingSaves"), false))
  {
    image.save(outputName);
    LOG(TRACE) << "Saved image to " << outputName;
  }
  else
  {
    LOG(TRACE) << "Skipping save in debug mode";
  }

  emit progress(100);
}

bool Executor::checkCancel()
{
  if (!QThread::currentThread()->isInterruptionRequested())
    return false;

  LOG(TRACE) << "Processing canceled";
  emit finished();
  return true;
}

QImage Executor::processOne(QImage const& source, OperationType const operation)
{
  TIMED_FUNC_IF(timer, config::qualified("logging.enhancements", false));

  switch(operation)
  {
  case OperationType::AutoWhiteBalance:
    return utils::convCvToImage(autoWhiteBalance(utils::convImageToCv(source, false)));
  }

  return phobos::utils::asserted::always;
}

void Executor::runInThread(QThread *thread, QProgressDialog *progressDialog)
{
  moveToThread(thread);

  if (progressDialog)
  {
    progressDialog->setRange(0, 100);
    progressDialog->setMinimumDuration(0);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setWindowFlags(progressDialog->windowFlags() &= ~Qt::WindowContextHelpButtonHint);
    progressDialog->setCancelButtonText(QObject::tr("Cancel"));

    QObject::connect(thread, &QThread::finished, progressDialog, &QProgressDialog::deleteLater);
    QObject::connect(this, &Executor::progress, progressDialog, &QProgressDialog::setValue);
    QObject::connect(this, &Executor::info, progressDialog, &QProgressDialog::setLabelText);
    QObject::connect(progressDialog, &QProgressDialog::canceled, thread, &QThread::requestInterruption);
  }

  QObject::connect(thread, &QThread::started, this, &Executor::run);
  QObject::connect(thread, &QThread::finished, this, &Executor::deleteLater);
  QObject::connect(thread, &QThread::finished, thread, &QThread::deleteLater);
  QObject::connect(this, &Executor::finished, thread, &QThread::quit);

  thread->start();
}

}}} // namespace phobos::iprocess::enhance
