#include "Widgets/PhotoItem/DetailsDialog.h"
#include "Widgets/PhotoItem/AddonRenderer.h"
#include "Widgets/ImageWidget.h"
#include "PhotoContainers/Item.h"
#include "PhotoContainers/ItemId.h"
#include "ImageProcessing/Metrics.h"
#include "Utils/LexicalCast.h"
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

template<typename Mem>
QString valueOrNull(iprocess::metric::Metric const& metric, Mem member, int const precision = 6)
{
  auto const& val = metric.*member;
  if (val == boost::none)
    return "null";

  QString result = QString::number(*val, 'f', precision);

  if (metric.seriesMetric)
  {
    auto const& seriesVal = (*metric.seriesMetric).*member;
    if (seriesVal != boost::none)
      result += QString(" (%1%)").arg(static_cast<int>(0.5 + 100.0 * *seriesVal));
  }

  return result;
}

QString depthOfFieldFormat(iprocess::metric::Metric const& metric)
{
  QString result = valueOrNull(metric, &iprocess::metric::Metric::depthOfField, 2);
  auto const& raw = metric.depthOfFieldRaw;

  if (metric.depthOfFieldRaw)
    result += QString(" (%1/%2/%3)").arg(raw->low, 0, 'f', 1)
                                    .arg(raw->median, 0, 'f', 1)
                                    .arg(raw->high, 0, 'f', 1);

  return result;
}

QString cameraString(pcontainer::FileInfo::CameraInfo const& camera)
{
  QString result;

  if (camera.make)
    result = camera.make.get();

  if (camera.make && camera.model)
    result += " / ";

  if (camera.model)
    result += camera.model.get();

  return result;
}

class DetailLayoutBuilder
{
public:
  DetailLayoutBuilder(pcontainer::Item const& photoItem,
                      QImage const& image,
                      iprocess::metric::MetricPtr const& metrics) :
    photoItem(photoItem), image(image), metrics(metrics),
    confPath("detailsDialog")
  {}

  void operator()(QWidget *dialog)
  {
    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

    buildFileinfoUi(mainLayout);
    buildMetricUi(mainLayout);

    dialog->setLayout(mainLayout);
  }

private:
  void buildFileinfoUi(QBoxLayout *parent)
  {
    auto const maxImgSize = config::qSize(confPath("image")("size"), QSize(100, 75));
    ImageWidget *imageWgt = new ImageWidget(image);
    imageWgt->setFixedSize(image.size().scaled(maxImgSize, Qt::KeepAspectRatio));

    QVBoxLayout *labelsLt = new QVBoxLayout();
    labelsLt->addWidget(new QLabel(QObject::tr("File: ") + photoItem.fileName()));
    labelsLt->addWidget(new QLabel(QObject::tr("Resolution: %1x%2")
                                               .arg(photoItem.info().size.width())
                                               .arg(photoItem.info().size.height())));
    labelsLt->addWidget(new QLabel(QObject::tr("Date: ")
                                   + QDateTime::fromSecsSinceEpoch(photoItem.info().timestamp).toString("ddd d MMMM yyyy, HH:mm:ss")));
    labelsLt->addWidget(new QLabel(QObject::tr("Camera: ") + cameraString(photoItem.info().camera)));

    QHBoxLayout *lt = new QHBoxLayout();
    lt->addWidget(imageWgt);
    lt->addLayout(labelsLt);
    lt->setAlignment(imageWgt, Qt::AlignTop);
    lt->setAlignment(labelsLt, Qt::AlignTop);
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

    using Metric = iprocess::metric::Metric;

    labelsLayout->addWidget(new QLabel(QObject::tr("Blur: ") + valueOrNull(*metrics, &Metric::blur, 1)));
    labelsLayout->addWidget(new QLabel(QObject::tr("Noise: ") + valueOrNull(*metrics, &Metric::noise, 3)));
    labelsLayout->addWidget(new QLabel(QObject::tr("Contrast: ") + valueOrNull(*metrics, &Metric::contrast, 3)));
    labelsLayout->addWidget(new QLabel(QObject::tr("Sharpness: ") + valueOrNull(*metrics, &Metric::sharpness, 2)));
    labelsLayout->addWidget(new QLabel(QObject::tr("Depth of field: ") + depthOfFieldFormat(*metrics)));
    labelsLayout->addWidget(new QLabel(QObject::tr("Saturation: ") + valueOrNull(*metrics, &Metric::saturation, 1)));
    labelsLayout->addWidget(new QLabel(QObject::tr("Complementary colors: ") + valueOrNull(*metrics, &Metric::complementary, 3)));
    labelsLayout->addStretch();

    QGroupBox *group = new QGroupBox(QObject::tr("Quality"));
    group->setLayout(labelsLayout);

    parent->addWidget(group);
  }

  void buildGraphicsUi(QBoxLayout *parent)
  {
    QVBoxLayout *layout = new QVBoxLayout();

    QLabel *histWgt = new QLabel();
    histWgt->setPixmap(buildHistogramPixmap());
    histWgt->setToolTip(QObject::tr("Histogram"));
    layout->addWidget(histWgt);

    QLabel *cummWgt = new QLabel();
    cummWgt->setPixmap(buildCumulativeHistogramPixmap());
    cummWgt->setToolTip(QObject::tr("Cumulative histogram"));
    layout->addWidget(cummWgt);

    if (metrics->hue)
    {
      QLabel *hueWgt = new QLabel();
      hueWgt->setPixmap(buildHueDisplay());
      hueWgt->setToolTip(QObject::tr("Hue channels"));
      layout->addWidget(hueWgt);
    }

    parent->addLayout(layout);
  }

  QPixmap buildHistogramPixmap()
  {
    auto const histConfig = confPath("histogram");
    QPixmap histPixmap(config::qSize(histConfig("size"), QSize(32, 32)));
    histPixmap.fill(config::qColor(histConfig("fillColor"), Qt::transparent));

    QPainter histPainter(&histPixmap);
    AddonRenderer(histPainter, confPath).histogram(metrics->histogram, histPixmap.size());
    return histPixmap;
  }

  QPixmap buildCumulativeHistogramPixmap()
  {
    auto const histConfig = confPath("cumulativeHistogram");
    QPixmap histPixmap(config::qSize(histConfig("size"), QSize(32, 32)));
    histPixmap.fill(config::qColor(histConfig("fillColor"), Qt::transparent));

    QPainter histPainter(&histPixmap);
    AddonRenderer(histPainter, confPath).cumulativeHistogram(metrics->histogram, histPixmap.size());
    return histPixmap;
  }

  QPixmap buildHueDisplay()
  {
    auto const hueConfig = confPath("hueDisplay");
    QPixmap huePixmap(config::qSize(hueConfig("size"), QSize(32, 32)));
    huePixmap.fill(config::qColor(hueConfig("fillColor"), Qt::transparent));

    QPainter huePainter(&huePixmap);
    AddonRenderer(huePainter, confPath).hueDisplay(*metrics->hue, huePixmap.size());
    return huePixmap;
  }

  pcontainer::Item const& photoItem;
  QImage const& image;
  iprocess::metric::MetricPtr const& metrics;
  config::ConfigPath const confPath;
};

class DetailsDialog : public QDialog
{
public:
  DetailsDialog(QWidget *parent,
                pcontainer::Item const& photoItem,
                QImage const& image,
                iprocess::metric::MetricPtr const& metrics) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    itemId(photoItem.id())
  {
    setAttribute(Qt::WA_DeleteOnClose);
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

void showDetailsDialog(QWidget *parent,
                       pcontainer::Item const& photoItem,
                       QImage const& image,
                       iprocess::metric::MetricPtr const& metrics)
{
  DetailsDialog *dialog = new DetailsDialog(parent, photoItem, image, metrics);

  dialog->show();
  dialog->raise();
  dialog->activateWindow();

  LOG(INFO) << "Showing details dialog for " << photoItem.id().toString();

  // leaking dialog because it will delete on close
}

}}} // namespace phobos::widgets::pitem
