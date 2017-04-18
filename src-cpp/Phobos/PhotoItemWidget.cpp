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
#include "PhotoItemWidget.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "ImageProcessing/ColoredPixmap.h"
#include "Utils/Algorithm.h"

namespace phobos {

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
// TODO: another addon: show file name
//
// TODO: show "Quality" text in quality label

PhotoItemWidget::PhotoItemWidget(pcontainer::ItemPtr const& photoItem,
                                 QImage const& preload,
                                 PhotoItemWidgetAddons const& addons) :
    ImageWidget(preload), _photoItem(photoItem), addons(addons)
{
    setFocusPolicy(Qt::StrongFocus);
    installEventFilter(this);

    QObject::connect(this, &ImageWidget::clicked, _photoItem.get(), &pcontainer::Item::toggleSelection);
    QObject::connect(_photoItem.get(), &pcontainer::Item::stateChanged,
                     this, static_cast<void (QWidget::*)()>(&QWidget::update));
}

namespace {
    QColor colorForState(pcontainer::ItemState const state)
    {
        switch(state)
        {
        case pcontainer::ItemState::UNKNOWN:
            return config::qColor("photoItemWidget.border.colorUnknown", Qt::lightGray);
        case pcontainer::ItemState::SELECTED:
            return config::qColor("photoItemWidget.border.colorSelected", Qt::green);
        case pcontainer::ItemState::DISCARDED:
            return config::qColor("photoItemWidget.border.colorDiscarded", Qt::red);
        }

        assert(false);
        return QColor();
    }
} // unnamed namespace

class PhotoItemWidget::PixmapRenderer
{
public:
    static std::string percentString(double const val, unsigned const decimalPlaces)
    {
        // TODO: use sprintf from QString
        std::ostringstream oss;
        oss << std::setprecision(decimalPlaces) << std::fixed << (val*100) << "%";
        return oss.str();
    }

    static QSize histogramSize(std::vector<float> const& data)
    {
        static std::initializer_list<unsigned> const POW2 = {2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
        assert(utils::valueIn(data.size(), POW2));

        unsigned width = config::qualified("photoItemWidget.histogram.width", 32);
        unsigned height = config::qualified("photoItemWidget.histogram.height", 32);
        if (!utils::valueIn(width, POW2)) width = 32;
        if (width > data.size()) width = data.size();
        if (!utils::valueIn(height, POW2) || height > data.size()) height = width;

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

    PixmapRenderer(PhotoItemWidget &widget) :
        borderWidth(config::qualified("photoItemWidget.border.width", 2u)),
        painter(&widget)
    {
        QSize const imageSize(widget.width() - 2*borderWidth, widget.height() - 2*borderWidth);
        QImage const scaledImage = widget.image().scaled(imageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        withBorderSize = QSize(scaledImage.width() + 2*borderWidth, scaledImage.height() + 2*borderWidth);

        painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing, true);
        painter.setWorldTransform(QTransform().translate((widget.width() - withBorderSize.width()) / 2,
                                                         (widget.height() - withBorderSize.height()) / 2));

        painter.fillRect(QRect(QPoint(), withBorderSize), colorForState(widget.photoItem().state()));
        painter.drawImage(borderWidth, borderWidth, scaledImage);
    }

    void focusMark()
    {
        alignedIcon("photoItemWidget.focusIcon", Qt::AlignRight | Qt::AlignTop);
    }

    void bestMark()
    {
        alignedIcon("photoItemWidget.bestMarkIcon", Qt::AlignTop | Qt::AlignLeft);
    }

    void scoreNum(double const scorePercent)
    {
        painter.save();
        painter.setOpacity(config::qualified("photoItemWidget.qualityText.opacity", 1u));
        painter.setPen(config::qColor("photoItemWidget.qualityText.color", Qt::black));
        painter.setFont(config::qFont("photoItemWidget.qualityText.font"));

        unsigned decimalPlaces = config::qualified("photoItemWidget.qualityText.decimalPlaces", 0u);
        std::string const text = percentString(scorePercent, decimalPlaces);

        unsigned const padding = config::qualified("photoItemWidget.qualityText.padding", 7u);
        painter.drawText(drawStartPoint(Qt::AlignLeft | Qt::AlignBottom, padding), text.c_str());

        painter.restore();
    }

    void ordNum(unsigned const ordNumber)
    {
        unsigned const width = config::qualified("photoItemWidget.ordNum.size", 20);
        QSize const size(width, width);
        QPoint const startPoint = drawStartPoint(Qt::AlignLeft | Qt::AlignTop, 0, size);
        QColor const backgroundColor = config::qColor("photoItemWidget.ordNum.background.color", Qt::transparent);

        if (backgroundColor != Qt::transparent)
        {
            painter.save();
            painter.setPen(backgroundColor);
            painter.setBrush(backgroundColor);
            painter.setOpacity(config::qualified("photoItemWidget.ordNum.background.opacity", 1.0));
            if (config::qualified("photoItemWidget.ordNum.background.type", std::string("")) == "circle")
                painter.drawEllipse(startPoint.x(), startPoint.y(), width, width);
            else
                painter.drawRect(startPoint.x(), startPoint.y(), width, width);
            painter.restore();
        }

        painter.save();
        painter.setFont(config::qFont("photoItemWidget.ordNum.font"));
        painter.setOpacity(config::qualified("photoItemWidget.ordNum.font.opacity", 1.0));
        painter.setPen(config::qColor("photoItemWidget.ordNum.font.color", Qt::black));
        painter.drawText(QRect(startPoint, size), Qt::AlignCenter, QString::number(ordNumber));

        painter.restore();
    }

    void histogram(std::vector<float> const& data)
    {
        QSize const histSize = histogramSize(data);
        auto const scaledHist = scaleHistogram(data, histSize.width());
        unsigned const padding = config::qualified("photoItemWidget.histogram.padding", 7u);

        painter.save();
        painter.setPen(config::qColor("photoItemWidget.histogram.color", Qt::black));
        painter.setOpacity(config::qualified("photoItemWidget.histogram.opacity", 1.0));

        double const H = histSize.height();
        QPoint const origin = drawStartPoint(Qt::AlignRight | Qt::AlignBottom, padding, histSize);
        for (std::size_t i = 0; i < scaledHist.size(); ++i)
            painter.drawLine(origin.x() + i, origin.y() + H,
                             origin.x() + i, origin.y() + (1.0-scaledHist[i])*H);

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

    void alignedIcon(std::string const& configTable, int const alignment)
    {
        unsigned const padding = config::qualified(configTable+".padding", 7u);
        QPixmap const pixmap = coloredIcon(configTable);
        painter.drawPixmap(drawStartPoint(alignment, padding, pixmap.size()), pixmap);
    }
// TODO: specify min and max sizes for icons, if possible keep min->percent->max size, if size exceeds
// pixmap, scale down
    QPixmap coloredIcon(std::string const& configTable)
    {
        double const sizePercent = config::qualified(configTable+".sizePercent", 0.2);
        QSize const iconSize(withBorderSize.width() * sizePercent,
                             withBorderSize.height() * sizePercent);
        QColor const color = config::qColor(configTable+".color", Qt::black);
        double const opacity = config::qualified(configTable+".opacity", 0.5);
        std::string const path = config::qualified(configTable+".path", std::string{});

        return iprocess::coloredPixmap(path, iconSize, color, opacity);
    }

    std::size_t const borderWidth;
    QSize withBorderSize;
    QPainter painter;
};

void PhotoItemWidget::paintEvent(QPaintEvent*)
{
    PixmapRenderer renderer(*this);

    if (addons.has(PhotoItemWidgetAddonType::FOCUS_IND) && hasFocus())
        renderer.focusMark();

    auto const seriesMetric = _photoItem->scoredMetric();
    if (addons.has(PhotoItemWidgetAddonType::SCORE_NUM) && seriesMetric)
        renderer.scoreNum(seriesMetric->score());

    if (addons.has(PhotoItemWidgetAddonType::BEST_IND) && seriesMetric && seriesMetric->bestQuality)
        renderer.bestMark();

    auto const metric = _photoItem->metric();
    if (addons.has(PhotoItemWidgetAddonType::HISTOGRAM) && metric && metric->histogram)
        renderer.histogram(*metric->histogram);

    if (addons.has(PhotoItemWidgetAddonType::ORD_NUM))
        renderer.ordNum(_photoItem->ord());
}

void PhotoItemWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu;

    if (_photoItem->state() == pcontainer::ItemState::UNKNOWN)
    {
        QObject::connect(menu.addAction("Select"), &QAction::triggered, _photoItem.get(), &pcontainer::Item::select);
        QObject::connect(menu.addAction("Discard"), &QAction::triggered, _photoItem.get(), &pcontainer::Item::discard);
    }
    else if (_photoItem->state() == pcontainer::ItemState::SELECTED)
    {
        QObject::connect(menu.addAction("Discard"), &QAction::triggered, _photoItem.get(), &pcontainer::Item::discard);
        QObject::connect(menu.addAction("Deselect"), &QAction::triggered, _photoItem.get(), &pcontainer::Item::deselect);
    }
    else if (_photoItem->state() == pcontainer::ItemState::DISCARDED)
    {
        QObject::connect(menu.addAction("Select"), &QAction::triggered, _photoItem.get(), &pcontainer::Item::select);
        QObject::connect(menu.addAction("Deselect"), &QAction::triggered, _photoItem.get(), &pcontainer::Item::deselect);
    }

    menu.addSeparator();

    QAction* viewSeries = menu.addAction("View series");
    QObject::connect(viewSeries, &QAction::triggered, this,
                     [this](){ emit openInSeries(_photoItem->seriesUuid()); });

    menu.exec(mapToGlobal(QPoint(event->x(), event->y())));
}

void PhotoItemWidget::focusInEvent(QFocusEvent*)
{
    update();
}

void PhotoItemWidget::focusOutEvent(QFocusEvent*)
{
    update();
}

bool PhotoItemWidget::eventFilter(QObject* object, QEvent* event)
{
    if (object != this)
        return false;

    QFocusEvent* focusEvent = dynamic_cast<QFocusEvent*>(event);
    if (!focusEvent)
        return false;

    return focusEvent->reason() == Qt::PopupFocusReason ||
           focusEvent->reason() == Qt::ActiveWindowFocusReason;
}

void PhotoItemWidget::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        _photoItem->toggleSelection();
    else
        ImageWidget::keyPressEvent(event);
}

} // namespace phobos
