#include "Widgets/ImageShowDialog.h"
#include "Widgets/ImageWidget.h"
#include "PhotoContainers/Item.h"
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
  ImageDialog(QWidget *parent, QImage const& image, QString const& title) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint),
    imageWidget(new ImageWidget(image))
  {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(title);
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

  void update(QImage const& image, QString const& title)
  {
    setWindowTitle(title);
    imageWidget->setImage(image);
    LOG(INFO) << "Updating separate image dialog for \"" << windowTitle() << '"';
  }

private:
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

void showImage(QWidget *parent, QImage const& image, pcontainer::Item const& photoItem)
{
  ImageDialog *dialog = findExistingDialog();
  if (dialog)
    dialog->update(image, photoItem.id().fileName);
  else
    dialog = new ImageDialog(parent, image, photoItem.id().fileName);

  dialog->show();
  dialog->raise();
}

bool exists()
{
  return findExistingDialog();
}

}}}// namespace phobos::widgets::fulldialog
