#include <QMenu>
#include <QAction>
#include <QPoint>
#include <QContextMenuEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QPixmap>
#include <QPainter>
#include <QFont>
#include "PhotoItemWidget.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "ImageProcessing/ColoredPixmap.h"

namespace phobos {

PhotoItemWidget::PhotoItemWidget(pcontainer::ItemPtr const& photoItem,
                                 std::shared_ptr<QPixmap> const& preload,
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
    static std::shared_ptr<QPixmap> filledPixmap(QSize const& size, QColor const& color)
    {
        auto pixmap = std::make_shared<QPixmap>(size);
        pixmap->fill(color);
        return pixmap;
    }
    static std::string percentString(double const val, unsigned const decimalPlaces)
    {
        unsigned whole = std::floor(val);
        std::string text = std::to_string(whole);
        if (decimalPlaces > 0)
        {
            text += ".";
            text += std::to_string(std::floor((val - whole) * std::pow(10, decimalPlaces)));
        }
        text += "%";
        return text;
    }

    PixmapRenderer(PhotoItemWidget const& widget) :
        borderWidth(config::qualified("photoItemWidget.border.width", 2u)),
        availableSizeWithoutBorders(widget.width() - 2*borderWidth, widget.height() - 2*borderWidth),
        scaledImagePixmap(widget.scaledPixmap(availableSizeWithoutBorders)),
        availableSizeWithBorders(scaledImagePixmap->width() + 2*borderWidth, scaledImagePixmap->height() + 2*borderWidth),
        targetPixmap(filledPixmap(availableSizeWithBorders, colorForState(widget.photoItem().state()))),
        painter(targetPixmap.get())
    {
        painter.drawPixmap((targetPixmap->width() - scaledImagePixmap->width()) / 2,
                           (targetPixmap->height() - scaledImagePixmap->height()) / 2,
                           *scaledImagePixmap);
    }

    void focusMark()
    {
        unsigned const padding = config::qualified("photoItemWidget.focusIcon.padding", 7u);
        QPixmap const focusPixmap = coloredIcon("photoItemWidget.focusIcon");
        painter.drawPixmap(targetPixmap->width() - borderWidth - padding - focusPixmap.width(),
                           borderWidth + padding,
                           focusPixmap);
    }

    void scoreNum(double const scorePercent)
    {
        painter.save();
        painter.setOpacity(config::qualified("photoItemWidget.qualityText.opacity", 1u));
        painter.setPen(config::qColor("photoItemWidget.qualityText.color", Qt::black));
        std::string const fontName = config::qualified("photoItemWidget.qualityText.font", std::string("Courier New"));
        painter.setFont(QFont(fontName.c_str()));

        unsigned const padding = config::qualified("photoItemWidget.qualityText.padding", 7u);
        QRectF const textRect(borderWidth + padding,
                              targetPixmap->height() - borderWidth - padding - painter.font().pointSize(),
                              1, 1);

        unsigned decimalPlaces = config::qualified("photoItemWidget.qualityText.decimalPlaces", 0u);
        std::string const text = percentString(scorePercent, decimalPlaces);
        painter.drawText(textRect, Qt::AlignLeft | Qt::TextSingleLine | Qt::TextDontClip, text.c_str());

        painter.restore();
    }

    std::shared_ptr<QPixmap> const& finish()
    {
        painter.end();
        return targetPixmap;
    }

private:
    QPixmap coloredIcon(std::string const& configTable)
    {
        double const sizePercent = config::qualified(configTable+".sizePercent", 0.2);
        QSize const iconSize(availableSizeWithoutBorders.width() * sizePercent,
                             availableSizeWithoutBorders.height() * sizePercent);
        QColor const color = config::qColor(configTable+".color", Qt::black);
        double const opacity = config::qualified(configTable+".opacity", 0.5);
        std::string const path = config::qualified(configTable+".path", std::string{});

        return iprocess::coloredPixmap(path, iconSize, color, opacity);
    }

    std::size_t const borderWidth;
    QSize const availableSizeWithoutBorders;
    std::shared_ptr<QPixmap> const scaledImagePixmap;
    QSize const availableSizeWithBorders;
    std::shared_ptr<QPixmap> targetPixmap;
    QPainter painter;
};

std::shared_ptr<QPixmap> PhotoItemWidget::renderedPixmap() const
{
    PixmapRenderer renderer(*this);

    if (addons.has(PhotoItemWidgetAddonType::FOCUS_IND) && hasFocus())
        renderer.focusMark();

    auto const metric = _photoItem->scoredMetric();
    if (addons.has(PhotoItemWidgetAddonType::SCORE_NUM) && metric)
        renderer.scoreNum(metric->score());

    return renderer.finish();
}

void PhotoItemWidget::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu menu;

    QAction* const toggleAction = menu.addAction(_photoItem->isSelected() ? "Discard" : "Select");
    QObject::connect(toggleAction, &QAction::triggered, _photoItem.get(), &pcontainer::Item::toggleSelection);

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
