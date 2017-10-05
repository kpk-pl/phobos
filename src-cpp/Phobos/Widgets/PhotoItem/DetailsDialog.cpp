#include "Widgets/PhotoItem/DetailsDialog.h"
#include "Widgets/PhotoItem/AddonRenderer.h"
#include "Widgets/ImageWidget.h"
#include "PhotoContainers/Item.h"
#include "PhotoContainers/ItemId.h"
#include "ImageProcessing/Metrics.h"
#include "ConfigExtension.h"
#include "ConfigPath.h"
#include <easylogging++.h>
#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPainter>
#include <QPixmap>

namespace phobos { namespace widgets { namespace pitem {

namespace {

class DetailLayoutBuilder
{
public:
  DetailLayoutBuilder(pcontainer::Item const& photoItem,
                      QImage const& image,
                      iprocess::MetricPtr const& metrics) :
    photoItem(photoItem), image(image), metrics(metrics),
    confPath("photoItemWidget.detailsDialog")
  {}

  void operator()(QWidget *dialog)
  {
    QVBoxLayout *mainLayout = new QVBoxLayout();
    buildFileinfoUi(mainLayout);
    buildMetricUi(mainLayout);
    mainLayout->addStretch();
    dialog->setLayout(mainLayout);
  }

private:
  void buildFileinfoUi(QBoxLayout *parent)
  {
    QHBoxLayout *lt = new QHBoxLayout();

    ImageWidget *imageWgt = new ImageWidget(image);
    imageWgt->setFixedSize(config::qSize(confPath, QSize(100, 75)));

    QVBoxLayout *labelsLt = new QVBoxLayout();
    labelsLt->addWidget(new QLabel("File: " + photoItem.fileName()));
    labelsLt->addWidget(new QLabel("Resolution: "));
    labelsLt->addWidget(new QLabel("Date: "));
    labelsLt->addStretch();

    lt->addWidget(imageWgt);
    lt->addLayout(labelsLt);
    parent->addLayout(lt);
  }

  void buildMetricUi(QBoxLayout *parent)
  {
    QHBoxLayout *lt = new QHBoxLayout();
    buildQualityUi(lt);
    lt->addStretch();
    buildGraphicsUi(lt);
    parent->addLayout(lt);
  }

  void buildQualityUi(QBoxLayout *parent)
  {
    QVBoxLayout *labelsLayout = new QVBoxLayout();

    labelsLayout->addWidget(new QLabel("Sharpness: "));

    QGroupBox *group = new QGroupBox("Quality");
    group->setLayout(labelsLayout);

    parent->addWidget(group);
  }

  void buildGraphicsUi(QBoxLayout *parent)
  {
    QPixmap histPixmap(config::qSize(confPath("histogramSize"), QSize(32, 32)));
    histPixmap.fill(config::qColor(confPath("histogramFill"), Qt::transparent));

    QPainter histPainter(&histPixmap);
    AddonRenderer(histPainter).histogram(metrics->histogram, histPixmap.size());

    QLabel *histWgt = new QLabel();
    histWgt->setPixmap(histPixmap);
    histWgt->setToolTip(QObject::tr("Histogram"));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(histWgt);
    parent->addLayout(layout);
  }

  pcontainer::Item const& photoItem;
  QImage const& image;
  iprocess::MetricPtr const& metrics;
  config::ConfigPath const confPath;
};

class DetailsDialog : public QDialog
{
public:
  DetailsDialog(pcontainer::Item const& photoItem,
                QImage const& image,
                iprocess::MetricPtr const& metrics) :
    itemId(photoItem.id())
  {
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setWindowTitle(itemId.fileName);
    DetailLayoutBuilder{photoItem, image, metrics}(this);
  }

  ~DetailsDialog()
  {
    LOG(INFO) << "Closing details dialog for " << itemId.toString();
  }

private:
  pcontainer::ItemId const itemId;
};
} // unnamed namespace

void showDetailsDialog(pcontainer::Item const& photoItem,
                       QImage const& image,
                       iprocess::MetricPtr const& metrics)
{
  DetailsDialog *dialog = new DetailsDialog(photoItem, image, metrics);

  dialog->show();
  dialog->raise();
  dialog->activateWindow();

  LOG(INFO) << "Showing details dialog for " << photoItem.id().toString();

  // leaking dialog because it will delete on close
}

}}} // namespace phobos::widgets::pitem
