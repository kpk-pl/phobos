#ifndef IMAGEPROCESSING_ENHANCE_EXECUTOR_H
#define IMAGEPROCESSING_ENHANCE_EXECUTOR_H

#include "ImageProcessing/Enhance/OperationType.h"
#include "PhotoContainers/ItemId.h"
#include <QImage>
#include <QString>
#include <QObject>
#include <vector>

class QThread;
class QProgressDialog;

namespace phobos { namespace iprocess { namespace enhance {

class Executor : public QObject
{
  Q_OBJECT

signals:
  void progress(int percent);
  void info(QString infoLabel);
  void finished();

public:
  explicit Executor(pcontainer::ItemId const& itemId,
                    std::vector<iprocess::enhance::OperationType> operations,
                    QString const outputName);

  void run();
  void runInThread(QThread *thread, QProgressDialog *progressDialog = nullptr);

  static QImage processOne(QImage const& source, OperationType const operation);

private:
  QImage readImage();
  void saveImage(QImage const& image);
  bool checkCancel();

  pcontainer::ItemId const itemId;
  std::vector<iprocess::enhance::OperationType> const operations;
  QString const outputName;
};

}}} // namespace phobos::iprocess::enhance

#endif // IMAGEPROCESSING_ENHANCE_EXECUTOR_H
