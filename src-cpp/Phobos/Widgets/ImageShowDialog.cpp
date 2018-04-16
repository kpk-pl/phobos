#include "Widgets/ImageShowDialog.h"
#include "Widgets/ImageWidget.h"
#include "ConfigExtension.h"
#include <easylogging++.h>
#include <QDialog>
#include <QVBoxLayout>
#include <QApplication>

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

    LOG(TRACE) << "Showing separate image dialog for \"" << windowTitle() << '"';
  }

  ~ImageDialog()
  {
    LOG(TRACE) << "Closing separate image dialog for \"" << windowTitle() << '"';
  }

  QString const& getCurrentPath() const { return currentPath; }

  void update(QImage const& image, QString const& path)
  {
    currentPath = path;
    setWindowTitle(currentPath);
    imageWidget->setImage(image);
    LOG(TRACE) << "Updating separate image dialog for \"" << windowTitle() << '"';
  }

  void updateCurrent(QImage const& image)
  {
    imageWidget->setImage(image);
    LOG(TRACE) << "Updating current image in separate dialog for \"" << windowTitle() << '"';
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
} // unnamed namespace

void showImage(QWidget *parent, QImage const& image, pcontainer::ItemId const& photoItem)
{
  ImageDialog *dialog = findExistingDialog();
  if (dialog)
    dialog->update(image, photoItem.fileName);
  else
    dialog = new ImageDialog(parent, image, photoItem.fileName);

  dialog->show();
  dialog->raise();
}

void updateImage(QImage const& image, pcontainer::ItemId const& photoItem)
{
  ImageDialog* dialog = findExistingDialog();
  if (dialog->getCurrentPath() != photoItem.fileName)
    return;

  dialog->updateCurrent(image);
}

bool exists()
{
  return findExistingDialog();
}

}}}// namespace phobos::widgets::fulldialog
