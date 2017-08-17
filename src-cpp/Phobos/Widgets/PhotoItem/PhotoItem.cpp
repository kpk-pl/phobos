#include <QMenu>
#include <QAction>
#include <QPoint>
#include <QContextMenuEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QPixmap>
#include <QPainter>
#include <QTransform>
#include <sstream>
#include "Widgets/PhotoItem/PhotoItem.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "ConfigPath.h"
#include "ImageProcessing/ColoredPixmap.h"
#include "ImageProcessing/Metrics.h"
#include "Utils/Algorithm.h"
#include "Utils/Asserted.h"
#include "Utils/ItemStateColor.h"
#include <easylogging++.h>

namespace phobos { namespace widgets { namespace pitem {

// TODO: bestmark, focus pixmaps -> CACHE IT by size!!! Those are calculated hundreds of times and the result is always the same!
// Time it before to make sure it has some impact
//
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
// TODO: another addon: show file name
// TODO: addon: cumulative histogram
// TODO: addon: diff from previous photo as a heat map like something?
// TODO: addon: vector flow from previous photo
// TODO: addon: Display selected metrics in text form on single series views.
// TODO: rightclick menu should open a dialog with all metrics displayed
//
// TODO: show "Quality" text in quality label
//
// TODO: Display image filename in tooltip or on the bar on the bottom of the app
//
// TODO: Right-click menu to open a dialog displaying all metrics for photo or whole series
//
// TODO: Tooltil on mouse over to display image filename
// TODO: Change focus icon color to blueish

PhotoItem::PhotoItem(pcontainer::ItemPtr const& photoItem, QImage const& preload, Addons const& addons, Capabilities const& capabilities) :
    ImageWidget(preload), _photoItem(photoItem), addons(addons), capabilities(capabilities)
{
    setFocusPolicy(Qt::StrongFocus);
    installEventFilter(this);

    QObject::connect(this, &ImageWidget::clicked, _photoItem.get(), &pcontainer::Item::toggleSelection);
    QObject::connect(_photoItem.get(), &pcontainer::Item::stateChanged,
                     this, static_cast<void (QWidget::*)()>(&QWidget::update));
}

class PhotoItem::PixmapRenderer
{
public:
    static std::string percentString(double const val, unsigned const decimalPlaces)
    {
        // TODO: use sprintf from QString
        std::ostringstream oss;
        oss << std::setprecision(decimalPlaces) << std::fixed << (val*100) << "%";
        return oss.str();
    }

    static QSize histogramSize(config::ConfigPath const& histConfig, iprocess::Histogram const& histogram)
    {
      static std::vector<unsigned> const POW2 = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

      std::size_t maxHistSize = 0;
      for (auto const& data : histogram.data)
      {
        assert(utils::valueIn(data.second.size(), POW2));
        maxHistSize = std::max(maxHistSize, data.second.size());
      }

      unsigned width = config::qualified(histConfig("width"), 32);
      unsigned height = config::qualified(histConfig("height"), 32);
      if (!utils::valueIn(width, POW2)) width = 32;
      if (width > maxHistSize) width = maxHistSize;
      if (!utils::valueIn(height, POW2) || height > maxHistSize) height = width;

      return QSize(width, height);
    }

    static std::vector<float> scaleHistogram(std::vector<float> const& data, std::size_t const width)
    {
        if (data.empty()) return data;

        unsigned const mult = data.size()/width;
        std::vector<float> accumulated;
        accumulated.reserve(width);

        for (std::size_t i = 0; i < width; ++i)
            accumulated.push_back(std::accumulate(data.begin()+i*mult, data.begin()+(i+1)*mult, 0.0));

        double const maxBin = *std::max_element(accumulated.begin(), accumulated.end());
        std::transform(accumulated.begin(), accumulated.end(), accumulated.begin(),
                [maxBin](double const v){ return v/maxBin; });

        return accumulated;
    }

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

    void focusMark()
    {
      alignedIcon(baseConfig("focusIcon"), Qt::AlignRight | Qt::AlignTop);
    }

    void bestMark()
    {
      alignedIcon(baseConfig("bestMarkIcon"), Qt::AlignTop | Qt::AlignLeft);
    }

    void scoreNum(double const scorePercent)
    {
      auto const textConfig = baseConfig + "qualityText";
      painter.save();
      painter.setOpacity(config::qualified(textConfig("opacity"), 1u));
      painter.setPen(config::qColor(textConfig("color"), Qt::black));
      painter.setFont(config::qFont(textConfig("font")));

      unsigned decimalPlaces = config::qualified(textConfig("decimalPlaces"), 0u);
      std::string const text = percentString(scorePercent, decimalPlaces);

      unsigned const padding = config::qualified(textConfig("padding"), 7u);
      painter.drawText(drawStartPoint(Qt::AlignLeft | Qt::AlignBottom, padding), text.c_str());

      painter.restore();
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
        painter.save();
        painter.setPen(backgroundColor);
        painter.setBrush(backgroundColor);
        painter.setOpacity(config::qualified(bgConfig("opacity"), 1.0));
        if (config::qualified(bgConfig("type"), std::string("")) == "circle")
          painter.drawEllipse(startPoint.x(), startPoint.y(), width, width);
        else
          painter.drawRect(startPoint.x(), startPoint.y(), width, width);
        painter.restore();
      }

      painter.save();
      painter.setFont(config::qFont(ordConfig("font")));
      painter.setOpacity(config::qualified(ordConfig("font")("opacity"), 1.0));
      painter.setPen(config::qColor(ordConfig("font")("color"), Qt::black));
      painter.drawText(QRect(startPoint, size), Qt::AlignCenter, QString::number(ordNumber));

      painter.restore();
    }

    void histogram(iprocess::Histogram const& histogram)
    {
      auto const histConfig = baseConfig + "histogram";

      painter.save();

      QSize const histSize = histogramSize(histConfig, histogram);
      unsigned const padding = config::qualified(histConfig("padding"), 7u);
      double const H = histSize.height();
      QPoint const origin = drawStartPoint(Qt::AlignRight | Qt::AlignBottom, padding, histSize);

      auto const drawData = [&](iprocess::Histogram::DataType const& data, config::ConfigPath const& binConfig){
        painter.setPen(config::qColor(binConfig("color"), Qt::black));
        painter.setOpacity(config::qualified(binConfig("opacity"), 1.0));
        auto const scaledHist = scaleHistogram(data, histSize.width());
        std::string const& type = config::qualified<std::string>(binConfig("style"), "none");

        if (type == "fill")
          for (std::size_t i = 0; i < scaledHist.size(); ++i)
            painter.drawLine(origin.x() + i, origin.y() + H,
                             origin.x() + i, origin.y() + (1.0-scaledHist[i])*H);
        else if (type == "line")
          for (std::size_t i = 1; i < scaledHist.size(); ++i)
            painter.drawLine(origin.x() + (i-1), origin.y() + (1.0-scaledHist[i-1])*H,
                             origin.x() + i,     origin.y() + (1.0-scaledHist[i])*H);
      };

      drawData(utils::asserted::fromMap(histogram.data, iprocess::Histogram::Channel::Value), histConfig("value"));
      drawData(utils::asserted::fromMap(histogram.data, iprocess::Histogram::Channel::Blue), histConfig("blue"));
      drawData(utils::asserted::fromMap(histogram.data, iprocess::Histogram::Channel::Green), histConfig("green"));
      drawData(utils::asserted::fromMap(histogram.data, iprocess::Histogram::Channel::Red), histConfig("red"));

      painter.restore();
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
// TODO: specify min and max sizes for icons, if possible keep min->percent->max size, if size exceeds
// pixmap, scale down
    QPixmap coloredIcon(config::ConfigPath const& configTable)
    {
      double const sizePercent = config::qualified(configTable("sizePercent"), 0.2);
      QSize const iconSize(withBorderSize.width() * sizePercent,
                           withBorderSize.height() * sizePercent);
      QColor const color = config::qColor(configTable("color"), Qt::black);
      double const opacity = config::qualified(configTable("opacity"), 0.5);
      std::string const path = config::qualified(configTable("path"), std::string{});

      return iprocess::coloredPixmap(path, iconSize, color, opacity);
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

    auto const metric = metrics();
    if (addons.has(AddonType::SCORE_NUM) && metric)
        renderer.scoreNum(metric->score());

    if (addons.has(AddonType::BEST_IND) && metric && metric->bestQuality)
        renderer.bestMark();

    if (addons.has(AddonType::HISTOGRAM) && metric && metric->histogram)
        renderer.histogram(metric->histogram);

    if (addons.has(AddonType::ORD_NUM))
        renderer.ordNum(_photoItem->ord(), metric && metric->bestQuality);
}

void PhotoItem::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu;

    QMenu* photoMenu = menu.addMenu("Photo");
    if (_photoItem->state() == pcontainer::ItemState::UNKNOWN)
    {
        QObject::connect(photoMenu->addAction("Select"), &QAction::triggered, _photoItem.get(), &pcontainer::Item::select);
        QObject::connect(photoMenu->addAction("Discard"), &QAction::triggered, _photoItem.get(), &pcontainer::Item::discard);
    }
    else if (_photoItem->state() == pcontainer::ItemState::SELECTED)
    {
        QObject::connect(photoMenu->addAction("Discard"), &QAction::triggered, _photoItem.get(), &pcontainer::Item::discard);
        QObject::connect(photoMenu->addAction("Deselect"), &QAction::triggered, _photoItem.get(), &pcontainer::Item::deselect);
    }
    else if (_photoItem->state() == pcontainer::ItemState::DISCARDED)
    {
        QObject::connect(photoMenu->addAction("Select"), &QAction::triggered, _photoItem.get(), &pcontainer::Item::select);
        QObject::connect(photoMenu->addAction("Deselect"), &QAction::triggered, _photoItem.get(), &pcontainer::Item::deselect);
    }

    if (capabilities.has(CapabilityType::REMOVE_PHOTO))
    {
      photoMenu->addSeparator();
      QObject::connect(photoMenu->addAction("Remove"), &QAction::triggered, this, [this](){ emit removeFromSeries(_photoItem->id()); });
    }

    QMenu* seriesMenu = menu.addMenu("Series");
    QObject::connect(seriesMenu->addAction("Select all"), &QAction::triggered, [this](){ emit changeSeriesState(_photoItem->seriesUuid(), pcontainer::ItemState::SELECTED); });
    QObject::connect(seriesMenu->addAction("Discard all"), &QAction::triggered, [this](){ emit changeSeriesState(_photoItem->seriesUuid(), pcontainer::ItemState::DISCARDED); });
    QObject::connect(seriesMenu->addAction("Deselect all"), &QAction::triggered, [this](){ emit changeSeriesState(_photoItem->seriesUuid(), pcontainer::ItemState::UNKNOWN); });

    menu.addSeparator();

    if (capabilities.has(CapabilityType::OPEN_SERIES))
    {
      QAction* viewSeries = menu.addAction("View series");
      QObject::connect(viewSeries, &QAction::triggered, [this](){ emit openInSeries(_photoItem->seriesUuid()); });
    }

    LOG(INFO) << "Displayed context menu for " << _photoItem->id().toString();
    menu.exec(mapToGlobal(QPoint(event->x(), event->y())));
}

void PhotoItem::focusInEvent(QFocusEvent*)
{
    update();
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

void PhotoItem::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        _photoItem->toggleSelection();
    else
        ImageWidget::keyPressEvent(event);
}

}}} // namespace phobos::widgets::pitem
