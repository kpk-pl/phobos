#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include <easylogging++.h>
#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include "ImportWizard/SeriesDisplayPage.h"
#include "ImportWizard/Types.h"
#include "Widgets/IconLabel.h"

#define TREEITEM_STANDARDSERIES 1
#define TREEITEM_LENGTHONESERIES 2
#define TREEITEM_STANDARDPHOTO 100

namespace phobos { namespace importwiz {

SeriesDisplayPage::SeriesDisplayPage(QWidget *parent) :
    QWizardPage(parent)
{
    setTitle(tr("Confirm series"));

    tree = new QTreeWidget();
    tree->header()->close();

    loadedStatusLabel = new QLabel();

    multipleLengthsInfo = new widgets::IconLabel(style()->standardIcon(QStyle::SP_MessageBoxInformation));
    multipleLengthsInfo->iconLabel()->setFixedSize(16, 16);

    lengthOneWarning = new widgets::IconLabel(style()->standardIcon(QStyle::SP_MessageBoxWarning));
    lengthOneWarning->iconLabel()->setFixedSize(16, 16);

    selectLengthOneButton = new QPushButton("Select back");
    QObject::connect(selectLengthOneButton, &QPushButton::clicked, this, &SeriesDisplayPage::selectBackSeriesWithOnePhoto);

    grid = new QGridLayout();
    grid->setColumnStretch(1, 1);
    grid->addWidget(loadedStatusLabel, 0, 0, 1, -1);
    grid->addWidget(multipleLengthsInfo, 1, 0, 1, 2);
    grid->addWidget(lengthOneWarning, 2, 0, 1, 2);
    grid->addWidget(selectLengthOneButton, 2, 2);
    grid->addWidget(tree, 3, 0, 1, -1);
    setLayout(grid);

    registerField("chosenSeries", this, "chosenSeries", SIGNAL(seriesChanged(PhotoSeriesVec)));
}

void SeriesDisplayPage::initializeInfoLabels(LengthCountMap const& lengthsCount)
{
    auto const oneIt = lengthsCount.find(1);
    if (oneIt != lengthsCount.end())
        initializeLengthOneWarning(oneIt->second);
    else
    {
        lengthOneWarning->hide();
        selectLengthOneButton->hide();
    }

    std::set<std::size_t> lengths;
    for (auto const& lc : lengthsCount)
        if (lc.first != 1)
            lengths.insert(lc.first);

    if (lengths.size() > 1)
        initializeMultipleLengthsInfo(lengths);
    else
        multipleLengthsInfo->hide();
}

void SeriesDisplayPage::initializeLengthOneWarning(std::size_t const count)
{
    LOG(INFO) << count << " series have just one photo";

    lengthOneWarning->show();
    lengthOneWarning->label()->setText(tr("%1 series with only one photo %2 been disabled")
            .arg(count).arg(count == 1 ? "has" : "have"));

    selectLengthOneButton->show();
}

void SeriesDisplayPage::initializeMultipleLengthsInfo(std::set<std::size_t> const& lengths)
{
    std::string const sLengthList =
            boost::algorithm::join(lengths | boost::adaptors::transformed(static_cast<std::string(*)(std::size_t)>(std::to_string)), ", ");

    LOG(INFO) << "Detected series with multiple different lengths: " << sLengthList;
    // TODO: When detecting series of multiples of lenghts, display warning that probably those can be divided in half.
    // Ex. series of length 7 and 21. Can be divided into 3 series.

    multipleLengthsInfo->show();
    multipleLengthsInfo->label()->setText(tr("Found series with different lengths: %1 photos").arg(sLengthList.c_str()));
}

void SeriesDisplayPage::initializePage()
{
    _dividedSeries = field("dividedSeries").value<PhotoSeriesVec>();
    LOG(DEBUG) << "Read " << _dividedSeries.size() << " series from previous dialog";

    std::size_t photoCount = 0;
    LengthCountMap seriesLengths;

    for (PhotoSeries const& series : _dividedSeries)
    {
        photoCount += series.size();
        seriesLengths[series.size()]++;

        int const seriesType = (series.size() == 1 ? TREEITEM_LENGTHONESERIES : TREEITEM_STANDARDSERIES);
        QTreeWidgetItem* seriesItem = new QTreeWidgetItem(tree, seriesType);
        seriesItem->setText(0, tr("%1 photos").arg(series.size()));
        seriesItem->setFlags(seriesItem->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsTristate | Qt::ItemIsEnabled);

        for (int i = 0; i < series.size(); ++i)
        {
            QTreeWidgetItem* photoItem = new QTreeWidgetItem(seriesItem, TREEITEM_STANDARDPHOTO);
            photoItem->setFlags(photoItem->flags() | Qt::ItemIsUserCheckable);
            photoItem->setText(0, tr("[%1] %2").arg(i).arg(series[i].fileName.c_str()));
            photoItem->setCheckState(0, (series.size() > 1) ? Qt::Checked : Qt::Unchecked);
        }
    }

    // TODO add info / warning / error icons to project and to those labels! maybe use system icons???
    // http://stackoverflow.com/questions/4453945/show-standard-warning-icon-in-qt4
    loadedStatusLabel->setText(tr("Loaded %1 photos into %2 series").arg(photoCount).arg(_dividedSeries.size()));
    LOG(INFO) << "Displayed " << photoCount  << " photos in " << _dividedSeries.size() << " series";

    // TODO: warning when it is probable that series were not divided correctly -> double (multiple) size series (like 7 and 14 photos)
    // Button to divide those again somehow

    initializeInfoLabels(seriesLengths);

    if (wizard()->button(QWizard::FinishButton))
        wizard()->button(QWizard::FinishButton)->setFocus();
}

bool SeriesDisplayPage::validatePage()
{
    _chosenSeries.clear();

    assert(_dividedSeries.size() == tree->topLevelItemCount());
    for (int seriesNum = 0; seriesNum < _dividedSeries.size(); ++seriesNum)
    {
        auto const& currentSeries = _dividedSeries[seriesNum];
        auto const& topLevel = tree->topLevelItem(seriesNum);
        assert(currentSeries.size() == topLevel->childCount());

        if (topLevel->checkState(0) == Qt::Checked)
            _chosenSeries.push_back(currentSeries);
        else if (topLevel->checkState(0) == Qt::PartiallyChecked)
        {
            PhotoSeries selectedPhotos;
            selectedPhotos.reserve(currentSeries.size());

            for (int photoNum = 0; photoNum < currentSeries.size(); ++photoNum)
                if (topLevel->child(photoNum)->checkState(0) == Qt::Checked)
                    selectedPhotos.push_back(currentSeries[photoNum]);

            assert(!selectedPhotos.empty());
            _chosenSeries.push_back(selectedPhotos);
        }
    }

    LOG(INFO) << "User selected " << _chosenSeries.size() << " series to load";
    emit seriesChanged(_chosenSeries);
    return true;
}

void SeriesDisplayPage::cleanupPage()
{
    QTreeWidgetItem *item;
    while ((item = tree->takeTopLevelItem(0)))
        delete item;

    _dividedSeries.clear();
    _chosenSeries.clear();
    emit seriesChanged(_chosenSeries);
}

void SeriesDisplayPage::selectBackSeriesWithOnePhoto()
{
    LOG(DEBUG) << "Selecting back series with just one photo";
    for (int i = 0; i < tree->topLevelItemCount(); ++i)
        if (tree->topLevelItem(i)->type() == TREEITEM_LENGTHONESERIES)
            tree->topLevelItem(i)->setCheckState(0, Qt::Checked);

    lengthOneWarning->hide();
    selectLengthOneButton->hide();
}

}} // namespace phobos::importwiz
