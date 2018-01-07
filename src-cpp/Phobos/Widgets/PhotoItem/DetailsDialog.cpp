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
#include <QStringBuilder>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QPainter>
#include <QPixmap>

namespace phobos { namespace widgets { namespace pitem {

namespace {

namespace detail {
template<typename M>
QString metricScore(iprocess::MetricPtr const& metric, M metricField)
{
  if (!metric)
    return "none";
  auto const val = ((*metric).*metricField).score();
  if (val == boost::none)
    return "none";

  return QString::number(*val * 100.0, 'f', 1) + "%";
}
} // namespace detail

template<typename M, typename S>
QLabel* metricLabel(iprocess::MetricPtr const& metric, M metricField, S seriesField, QString const name, int const precision)
{
  QLabel* result = new QLabel(name + ": " + detail::metricScore(metric, metricField));
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
                      iprocess::MetricPtr const& metrics) :
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

    using namespace iprocess;

    labelsLayout->addWidget(metricLabel(metrics, &Metric::blur, &MetricSeriesScores::blur, "Blur", 1));
    labelsLayout->addWidget(metricLabel(metrics, &Metric::noise, &MetricSeriesScores::noise, "Noise", 3));
    labelsLayout->addWidget(metricLabel(metrics, &Metric::contrast, &MetricSeriesScores::contrast, "Contrast", 3));
    labelsLayout->addWidget(metricLabel(metrics, &Metric::sharpness, &MetricSeriesScores::sharpness, "Sharpness", 2));
    labelsLayout->addWidget(metricLabel(metrics, &Metric::depthOfField, &MetricSeriesScores::depthOfField, "Depth of field", 2));
    labelsLayout->addWidget(metricLabel(metrics, &Metric::saturation, &MetricSeriesScores::saturation, "Saturation", 1));
    labelsLayout->addWidget(metricLabel(metrics, &Metric::complementary, &MetricSeriesScores::complementary, "Complementary colors", 3));
    labelsLayout->addStretch();

    QGroupBox *group = new QGroupBox(QObject::tr("Quality"));
    group->setLayout(labelsLayout);

    parent->addWidget(group);
  }

  void buildGraphicsUi(QBoxLayout *parent)
  {
    if (!metrics)
      return;

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
  iprocess::MetricPtr const& metrics;
  config::ConfigPath const confPath;
};

class DetailsDialog : public QDialog
{
public:
  DetailsDialog(QWidget *parent,
                pcontainer::Item const& photoItem,
                QImage const& image,
                iprocess::MetricPtr const& metrics) :
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
                       iprocess::MetricPtr const& metrics)
{
  DetailsDialog *dialog = new DetailsDialog(parent, photoItem, image, metrics);

  dialog->show();
  dialog->raise();
  dialog->activateWindow();

  LOG(INFO) << "Showing details dialog for " << photoItem.id().toString();

  // leaking dialog because it will delete on close
}

}}} // namespace phobos::widgets::pitem
