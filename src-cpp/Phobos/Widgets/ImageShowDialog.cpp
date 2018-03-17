#include "Widgets/ImageShowDialog.h"
#include "Widgets/ImageWidget.h"
#include "PhotoContainers/Item.h"
#include "ConfigExtension.h"
#include <easylogging++.h>
#include <QDialog>
#include <QVBoxLayout>
#include <QApplication>
#include <QRunnable>
#include <QThreadPool>
#include <QImageReader>

namespace phobos { namespace widgets { namespace fulldialog {

namespace {
static QString const WINDOW_NAME = "FullScreenImageDialog";

class ImageDialog : public QDialog
{
public:
  ImageDialog(QWidget *parent, QImage const& image, QString const& path) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint),
    currentPath(path),
    imageWidget(new ImageWidget(image))
  {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(currentPath);
    setObjectName(WINDOW_NAME);

    QVBoxLayout* box = new QVBoxLayout;
    box->setContentsMargins(0, 0, 0, 0);
    box->addWidget(imageWidget);

    setLayout(box);
    resize(config::qSize("fullscreenWindow.defaultSize", QSize(1024, 768)));

    LOG(INFO) << "Showing separate image dialog for \"" << windowTitle() << '"';
  }

  ~ImageDialog()
  {
    LOG(INFO) << "Closing separate image dialog for \"" << windowTitle() << '"';
  }

  QString const& getCurrentPath() const { return currentPath; }

  void update(QImage const& image, QString const& path)
  {
    currentPath = path;
    setWindowTitle(currentPath);
    imageWidget->setImage(image);
    LOG(INFO) << "Updating separate image dialog for \"" << windowTitle() << '"';
  }

  void updateCurrent(QImage const& image)
  {
    imageWidget->setImage(image);
    LOG(INFO) << "Updating current image in separate dialog for \"" << windowTitle() << '"';
  }

private:
  QString currentPath;
  ImageWidget *imageWidget;
};

ImageDialog* findExistingDialog()
{
  for (QWidget* wgt : QApplication::topLevelWidgets())
    if (wgt->objectName() == WINDOW_NAME)
      return dynamic_cast<ImageDialog*>(wgt);
  return nullptr;
}

// TODO. This is a workaround
// Need to involve cache with proactive (!!!) loading, show big quality from cache
// And then still load full quality to update
// But first guess should be from cache
struct LoadingThread : public QRunnable
{
  QString const _path;

  LoadingThread(QString const& path) :
    _path(path)
  {}

  void run() override
  {
    QImageReader reader(_path);
    reader.setAutoTransform(true);
    reader.setAutoDetectImageFormat(true);

    QImage fullImage = reader.read();
    if (fullImage.isNull())
      return;

    ImageDialog *dialog = findExistingDialog();
    if (!dialog)
      return;

    if (dialog->getCurrentPath() == _path)
      dialog->updateCurrent(fullImage);
  }
};
} // unnamed namespace

void showImage(QWidget *parent, QImage const& image, pcontainer::Item const& photoItem)
{
  ImageDialog *dialog = findExistingDialog();
  if (dialog)
    dialog->update(image, photoItem.id().fileName);
  else
    dialog = new ImageDialog(parent, image, photoItem.id().fileName);

  dialog->show();
  dialog->raise();

  QRunnable* loadingJob = new LoadingThread(photoItem.id().fileName);
  loadingJob->setAutoDelete(true);
  QThreadPool::globalInstance()->start(loadingJob);
}

bool exists()
{
  return findExistingDialog();
}

}}}// namespace phobos::widgets::fulldialog
