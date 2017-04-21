#include <functional>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QEvent>
#include <QKeyEvent>
#include <QPixmap>
#include "AllSeriesView.h"
#include "Config.h"
#include "ConfigExtension.h"
#include "PhotoItemWidget.h"
#include "PhotoItemWidgetAddon.h"
#include "Utils/Algorithm.h"
#include "Utils/Focused.h"

namespace phobos {

// TODO: Rightclick menu option to remove series entirely
// think how this can be done, maybe removing objects from grid is enough to leave empty row ?
// But this will mess up with numbering
//
// TODO: Add series numbering
// left to each photo should be a number, colorcode series that have at least one photo chosen

namespace {
class ArrowFilter : public QObject
{
public:
    ArrowFilter(QObject* parent = nullptr) :
        QObject(parent)
    {}

    bool eventFilter(QObject* watched, QEvent* event) override
    {
        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
            auto const& key = keyEvent->key();
            if (key == Qt::Key_Down || key == Qt::Key_Up ||
                key == Qt::Key_Left || key == Qt::Key_Right)
            {
                event->ignore();
                return true;
            }
        }
        return QObject::eventFilter(watched, event);
    }
};
} // unnamed namespace

struct AllSeriesView::Coords
{
    template<typename T, typename U>
    Coords(T row, U col) :
        row(int(row)), col(int(col))
    {}
    static const int MAX = std::numeric_limits<int>::max();
    int row, col;
};

AllSeriesView::AllSeriesView()
{
    // TODO: navigationBar

    grid = new QGridLayout();
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setHorizontalSpacing(config::get()->get_qualified_as<unsigned>("allSeriesView.photoSpacing").value_or(3));
    grid->setVerticalSpacing(config::get()->get_qualified_as<unsigned>("allSeriesView.seriesSpacing").value_or(15));

    QVBoxLayout* scrollLayout = new QVBoxLayout();
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->addLayout(grid);
    scrollLayout->addStretch();

    QWidget* scrollWidget = new QWidget();
    scrollWidget->setLayout(scrollLayout);

    QScrollArea* scroll = new QScrollArea();
    scroll->installEventFilter(new ArrowFilter(scroll));
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidget(scrollWidget);

    QVBoxLayout* vlayout = new QVBoxLayout();
    // TODO: vlayout->addWidget(navigationBar);
    vlayout->addWidget(scroll);

    setLayout(vlayout);
}

void AllSeriesView::focusSeries()
{
    if (numberOfSeries() > 0)
        grid->itemAtPosition(0, 0)->widget()->setFocus();
}

void AllSeriesView::focusSeries(QUuid const& seriesUuid)
{
    assert(utils::valueIn(seriesUuid, seriesUuidToRow));
    grid->itemAtPosition(seriesUuidToRow[seriesUuid], 0)->widget()->setFocus();
}

namespace {
    QSize preferredSize()
    {
        return config::qSize("allSeriesView.pixmapSize").value_or(QSize(320, 240));
    }

    QImage buildPreloadImage()
    {
        QPixmap pixmap(preferredSize());
        pixmap.fill(Qt::lightGray);
        return pixmap.toImage();
    }

    QImage getPreloadImage()
    {
        // TODO: handle when size changes
        static QImage const image = buildPreloadImage();
        return image;
    }
} // unnamed namespace

void AllSeriesView::addNewSeries(pcontainer::SeriesPtr series)
{
    std::size_t const row = numberOfSeries();
    seriesUuidToRow.emplace(series->uuid(), row);

    for (std::size_t col = 0; col < series->size(); ++col)
    {
        PhotoItemWidget* item = new PhotoItemWidget(series->item(col), getPreloadImage(),
            PhotoItemWidgetAddons(config::get()->get_qualified_array_of<std::string>("allSeriesView.enabledAddons").value_or({})));

        QObject::connect(item, &PhotoItemWidget::openInSeries, this,
          [this](QUuid const& uuid){
            switchView(ViewDescription::make(ViewType::ANY_SINGLE_SERIES, uuid));
          }
        );

        grid->addWidget(item, row, col);
        series->item(col)->loadPhoto(preferredSize(), item, std::bind(&PhotoItemWidget::setImage, item, std::placeholders::_1));
    }
}

void AllSeriesView::keyPressEvent(QKeyEvent* keyEvent)
{
    if (keyEvent->type() == QEvent::KeyPress &&
            utils::valueIn(keyEvent->key(), {Qt::Key_Left, Qt::Key_Right, Qt::Key_Up, Qt::Key_Down}))
    {
        auto const focusCoords = focusGridCoords();
        if (focusCoords)
        {
            Coords const jump = findValidProposal(nextJumpProposals(*focusCoords, keyEvent->key()));
            grid->itemAtPosition(jump.row, jump.col)->widget()->setFocus();
        }
        else if (grid->count() > 0)
            grid->itemAtPosition(0, 0)->widget()->setFocus();
    }

    QWidget::keyPressEvent(keyEvent);
}

boost::optional<AllSeriesView::Coords> AllSeriesView::focusGridCoords() const
{
    PhotoItemWidget* focusItem = utils::focusedPhotoItemWidget();
    if (!focusItem)
        return boost::none;

    QUuid const& focusSeries = focusItem->photoItem().seriesUuid();
    assert(utils::valueIn(focusSeries, seriesUuidToRow));

    unsigned const focusRow = seriesUuidToRow.find(focusSeries)->second;
    for (int i = 0; i < grid->columnCount(); ++i)
    {
        auto const& widgetItem = grid->itemAtPosition(focusRow, i);
        if (!widgetItem || !widgetItem->widget())
            continue;
        if (widgetItem->widget() == focusItem)
            return Coords{focusRow, i};
    }

    assert(false); // should ALWAYS find item in the loop
    return boost::none;
}

std::vector<AllSeriesView::Coords>
    AllSeriesView::nextJumpProposals(Coords const& coords, int const directionKey) const
{
    unsigned const row = coords.row;
    unsigned const col = coords.col;
    unsigned const maxRow = grid->rowCount()-1;

    switch(directionKey)
    {
    case Qt::Key_Right:
        return {{row, col+1}, {row+1, 0}, {0, 0}};
    case Qt::Key_Down:
        return {{row+1, col}, {row+1, Coords::MAX}, {0, col}, {0, Coords::MAX}};
    case Qt::Key_Left:
        return {{row, col-1}, {row-1, Coords::MAX}, {maxRow, Coords::MAX}};
    case Qt::Key_Up:
        return {{row-1, col}, {row-1, Coords::MAX}, {maxRow, col}, {maxRow, Coords::MAX}};
    }

    assert(false);
    return {};
}

AllSeriesView::Coords AllSeriesView::findValidProposal(std::vector<Coords> const& proposals) const
{
    assert(grid->count() > 0); // Should never be called on empty grid

    for (Coords const& c : proposals)
    {
        if (c.row < 0 || c.row >= grid->rowCount())
            continue;
        if (c.col < 0 || (c.col >= grid->columnCount() && c.col != Coords::MAX))
            continue;
        if (c.col == Coords::MAX)
        {
            for (int i = 0; i <= grid->columnCount(); ++i)
                if (!grid->itemAtPosition(c.row, i))
                    if (i != 0)
                        return {c.row, i-1};
                    break;
        }
        else
        {
            if (grid->itemAtPosition(c.row, c.col))
                return {c.row, c.col};
        }
    }

    /*
     * Should never happen if proposals are correctly constructed.
     * On each proposal list should be at least one (0,0) or (0, None). When grid has at least one
     * correctly positioned element at (0,0) it should be found.
     */
    assert(false); // No valid proposals found
    return {0, 0};
}

} // namespace phobos