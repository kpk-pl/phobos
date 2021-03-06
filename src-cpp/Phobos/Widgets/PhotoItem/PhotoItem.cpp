#include "Widgets/PhotoItem/PhotoItem.h"
#include "Widgets/PhotoItem/DetailsDialog.h"
#include "Widgets/PhotoItem/AddonRenderer.h"
#include "Widgets/ImageShowDialog.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "ConfigPath.h"
#include "ImageProcessing/Utils/ColoredPixmap.h"
#include "ImageProcessing/Metrics.h"
#include "Utils/Algorithm.h"
#include "Utils/Asserted.h"
#include "Utils/ItemStateColor.h"
#include "Utils/PainterFrame.h"
#include <easylogging++.h>
#include <QMenu>
#include <QAction>
#include <QPoint>
#include <QContextMenuEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QPixmap>
#include <QPainter>
#include <QTransform>
#include <QPixmapCache>

namespace phobos { namespace widgets { namespace pitem {

// TODO: histogram -> maybe assume it will never change
// maybe assume histogram size will never change
// then cache histograms scaled to buckets inside photoitemwidget
// but first TIME how long does it take to calculate new buckets
// and to draw them
// because pixmaps could be cached as well
// just using differend qpainter, save rendered pixmaps in photoitemwidget for later use
//
// TODO: from config, enable setBackground(color) for text, enable opacity
//
// TODO: display max scores as in "Quality: 80%/94%". 100% is not always maximum
// TODO: addon: diff from previous photo as a heat map like something?
// TODO: addon: vector flow from previous photo
// TODO: addon: Display selected metrics in text form on single series views.
//
// TODO: show "Quality" text in quality label
//
// TODO: color border is not enough. need to add a triangle in one of the corners in color
// or color ord number
// or use some ico like questionmark, tick and a cross

PhotoItem::PhotoItem(pcontainer::ItemPtr const& photoItem,
                     Addons const& addons,
                     Capabilities const& capabilities) :
  ImageWidget(photoItem->info().size),
  _photoItem(photoItem),
  addons(addons),
  capabilities(capabilities)
{
  setFocusPolicy(Qt::StrongFocus);
  setToolTip(photoItem->fileName());
  installEventFilter(this);

  QObject::connect(this, &ImageWidget::clicked, _photoItem.get(), &pcontainer::Item::invert);
  QObject::connect(_photoItem.get(), &pcontainer::Item::stateChanged,
                   this, static_cast<void (QWidget::*)()>(&QWidget::update));
}

class PhotoItem::PixmapRenderer
{
public:
    PixmapRenderer(PhotoItem &widget) :
      borderWidth(config::qualified(baseConfig("border")("width"), 2u)),
      painter(&widget)
    {
        QSize const imageSize(widget.width() - 2*borderWidth, widget.height() - 2*borderWidth);
        QImage const scaledImage = widget.image().scaled(imageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        withBorderSize = QSize(scaledImage.width() + 2*borderWidth, scaledImage.height() + 2*borderWidth);

        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
        painter.setWorldTransform(QTransform().translate((widget.width() - withBorderSize.width()) / 2,
                                                         (widget.height() - withBorderSize.height()) / 2));

        painter.fillRect(QRect(QPoint(), withBorderSize), utils::itemStateColor(widget.photoItem().state()));
        painter.drawImage(borderWidth, borderWidth, scaledImage);
    }

    // TODO: Focus eye must be colorfull icon
    void focusMark()
    {
      if (config::qualified(baseConfig("focusIcon")("enabled"), true))
        alignedIcon(baseConfig("focusIcon"), Qt::AlignRight | Qt::AlignTop);
    }

    void bestMark()
    {
      alignedIcon(baseConfig("bestMarkIcon"), Qt::AlignTop | Qt::AlignLeft);
    }

    void scoreNum(boost::optional<double> const scorePercent)
    {
      if (!scorePercent)
        return;

      auto const textConfig = baseConfig("qualityText");

      utils::PainterFrame frame(painter);
      painter.setOpacity(config::qualified(textConfig("opacity"), 1u));
      painter.setPen(config::qColor(textConfig("color"), Qt::black));
      painter.setFont(config::qFont(textConfig("font")));

      unsigned decimalPlaces = config::qualified(textConfig("decimalPlaces"), 0u);
      QString const text = QString("%1%").arg(scorePercent.get()*100.0, 0, 'f', decimalPlaces);

      unsigned const padding = config::qualified(textConfig("padding"), 7u);
      painter.drawText(drawStartPoint(Qt::AlignLeft | Qt::AlignBottom, padding), text);
    }

    void ordNum(unsigned const ordNumber, bool best)
    {
      auto const ordConfig = baseConfig + "ordNum";
      auto const bgConfig = ordConfig + (best ? "bestBackground" : "background");

      unsigned const width = config::qualified(ordConfig("size"), 20);
      QSize const size(width, width);
      QPoint const startPoint = drawStartPoint(Qt::AlignLeft | Qt::AlignTop, 0, size);
      QColor const backgroundColor = config::qColor(bgConfig("color"), Qt::transparent);

      if (backgroundColor != Qt::transparent)
      {
        utils::PainterFrame frame(painter);
        painter.setPen(backgroundColor);
        painter.setBrush(backgroundColor);
        painter.setOpacity(config::qualified(bgConfig("opacity"), 1.0));
        if (config::qualified(bgConfig("type"), std::string("")) == "circle")
          painter.drawEllipse(startPoint.x(), startPoint.y(), width, width);
        else
          painter.drawRect(startPoint.x(), startPoint.y(), width, width);
      }

      utils::PainterFrame frame(painter);
      painter.setFont(config::qFont(ordConfig("font")));
      painter.setOpacity(config::qualified(ordConfig("font")("opacity"), 1.0));
      painter.setPen(config::qColor(ordConfig("font")("color"), Qt::black));
      painter.drawText(QRect(startPoint, size), Qt::AlignCenter, QString::number(ordNumber));
    }

    void histogram(iprocess::feature::Histogram const& histogram)
    {
      auto const histConfig = baseConfig("histogram");

      auto const minWgtSize = config::qSize(histConfig("minWidgetSize"), QSize());
      if (minWgtSize.isValid() && (withBorderSize.width() < minWgtSize.width() || withBorderSize.height() < minWgtSize.height()))
        return;

      auto const prefferedSize = config::qSize(histConfig("size"), QSize(32, 32));
      unsigned const padding = config::qualified(histConfig("padding"), 7u);

      QPoint const origin = drawStartPoint(Qt::AlignRight | Qt::AlignBottom, padding, prefferedSize);
      AddonRenderer renderer(painter, baseConfig);
      renderer.histogram(histogram, prefferedSize, origin);
    }

private:
    QPoint drawStartPoint(int const alignment, unsigned const padding, QSize const& pixmapSize = QSize()) const
    {
        QPoint result;

        if (alignment & Qt::AlignLeft)
            result.setX(borderWidth + padding);
        else
            result.setX(withBorderSize.width() - borderWidth - padding - pixmapSize.width());

        if (alignment & Qt::AlignTop)
            result.setY(borderWidth + padding);
        else
            result.setY(withBorderSize.height() - borderWidth - padding - pixmapSize.height());

        return result;
    }

    void alignedIcon(config::ConfigPath const& configTable, int const alignment)
    {
      unsigned const padding = config::qualified(configTable("padding"), 7u);
      QPixmap const pixmap = coloredIcon(configTable);
      painter.drawPixmap(drawStartPoint(alignment, padding, pixmap.size()), pixmap);
    }

    QPixmap coloredIcon(config::ConfigPath const& configTable)
    {
      double const opacity = config::qualified(configTable("opacity"), 0.5);

      QSize const minSize = config::qSize(configTable("minSize"), QSize());
      QSize const maxSize = config::qSize(configTable("maxSize"), QSize());
      QSize iconSize = withBorderSize * config::qualified(configTable("sizePercent"), 0.2);
      if (minSize.isValid() && (minSize.width() > iconSize.width() || minSize.height() > iconSize.height()))
        iconSize = iconSize.scaled(minSize, Qt::KeepAspectRatioByExpanding);
      else if (maxSize.isValid() && (maxSize.width() < iconSize.width() || maxSize.height() < iconSize.height()))
        iconSize = iconSize.scaled(maxSize, Qt::KeepAspectRatio);

      QString const cacheKey = QString::fromStdString(configTable) + QString("%2x%3").arg(iconSize.width()).arg(iconSize.height());

      QPixmap result;
      if (!QPixmapCache::find(cacheKey, &result) || result.isNull())
      {
        result = iprocess::utils::coloredPixmap(configTable, iconSize, opacity);
        QPixmapCache::insert(cacheKey, result);
      }
      return result;
    }

    static config::ConfigPath const baseConfig;
    std::size_t const borderWidth;
    QSize withBorderSize;
    QPainter painter;
};

config::ConfigPath const PhotoItem::PixmapRenderer::baseConfig("photoItemWidget");

void PhotoItem::paintEvent(QPaintEvent*)
{
  PixmapRenderer renderer(*this);

  if (addons.has(AddonType::FOCUS_IND) && hasFocus())
    renderer.focusMark();

  // TODO: Maybe different font for series score and single score?
  auto const metric = metrics();
  if (addons.has(AddonType::SCORE_NUM) && metric)
  {
    if (!_photoItem->isFromASeries)
      renderer.scoreNum(metric->score());
    else if (metric->seriesScores)
      renderer.scoreNum(metric->seriesScores->score());
  }

  if (addons.has(AddonType::BEST_IND) && metric && metric->seriesScores && metric->seriesScores->bestQuality)
    renderer.bestMark();

  if (addons.has(AddonType::HISTOGRAM) && metric && metric->histogram)
    renderer.histogram(metric->histogram);

  if (addons.has(AddonType::ORD_NUM))
    renderer.ordNum(_photoItem->ord(), metric && metric->seriesScores && metric->seriesScores->bestQuality);
}

void PhotoItem::contextMenuEvent(QContextMenuEvent* event)
{
  // TODO: show context menu ONLY if click happened on the drawed canvas. Don't show it if
  // click was on the spacer between photos of different sizes
  // This has to be configurable in config
  QMenu menu;

  QMenu* photoMenu = menu.addMenu("Photo");
  if (_photoItem->state() != pcontainer::ItemState::SELECTED)
    photoMenu->addAction(tr("Select"), _photoItem.get(), &pcontainer::Item::select);
  if (_photoItem->state() != pcontainer::ItemState::IGNORED)
    photoMenu->addAction(tr("Deselect"), _photoItem.get(), &pcontainer::Item::ignore);

  if (capabilities.has(CapabilityType::REMOVE_PHOTO))
  {
    photoMenu->addSeparator();
    photoMenu->addAction(tr("Remove"), [this](){ emit removeFromSeries(_photoItem->id()); });
  }

  QMenu* seriesMenu = menu.addMenu("Series");
  seriesMenu->addAction(tr("Select all"), [this](){ emit changeSeriesState(_photoItem->seriesUuid(), pcontainer::ItemState::SELECTED); });
  seriesMenu->addAction(tr("Deselect all"), [this](){ emit changeSeriesState(_photoItem->seriesUuid(), pcontainer::ItemState::IGNORED); });

  if (capabilities.has(CapabilityType::REMOVE_SERIES))
  {
    seriesMenu->addSeparator();
    seriesMenu->addAction(tr("Remove"), [this](){ emit removeAllSeries(_photoItem->seriesUuid()) ;});
  }

  if (capabilities.has(CapabilityType::OPEN_SERIES))
  {
    menu.addSeparator();
    menu.addAction(tr("View series"), this, &PhotoItem::openInSeries);
  }

  menu.addSeparator();
  menu.addAction(tr("Show details"), this, &PhotoItem::openDetailsDialog);
  menu.addAction(tr("Fullscreen"), this, [this](){ emit showFullscreen(photoItem().id()); });

  LOG(TRACE) << "Displayed context menu for " << _photoItem->id().toString();
  menu.exec(mapToGlobal(QPoint(event->x(), event->y())));
}

void PhotoItem::openDetailsDialog() const
{
  showDetailsDialog(window(), *_photoItem, image(), metrics());
}

void PhotoItem::focusInEvent(QFocusEvent*)
{
  update();
  if (fulldialog::exists())
    emit showFullscreen(photoItem().id());
}

void PhotoItem::focusOutEvent(QFocusEvent*)
{
  update();
}

bool PhotoItem::eventFilter(QObject* object, QEvent* event)
{
    if (object != this)
        return false;

    QFocusEvent* focusEvent = dynamic_cast<QFocusEvent*>(event);
    if (!focusEvent)
        return false;

    return focusEvent->reason() == Qt::PopupFocusReason ||
           focusEvent->reason() == Qt::ActiveWindowFocusReason;
}

// TODO:Invert selection only if widget had focus.
// not sure how to check that since keyPressEvent is handled after widget receives focus
void PhotoItem::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
    _photoItem->invert();
  else
    ImageWidget::keyPressEvent(event);
}

}}} // namespace phobos::widgets::pitem
