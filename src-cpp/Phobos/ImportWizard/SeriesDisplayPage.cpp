#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QHeaderView>
#include "ImportWizard/SeriesDisplayPage.h"
#include "ImportWizard/Types.h"

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

    lengthOneWarning = new QLabel();
    lengthOneWarning->hide();
    selectLengthOneButton = new QPushButton("Select back");
    selectLengthOneButton->hide();
    QObject::connect(selectLengthOneButton, &QPushButton::clicked, this, &SeriesDisplayPage::selectBackSeriesWithOnePhoto);

    QGridLayout* grid = new QGridLayout();
    grid->setColumnStretch(1, 1);
    grid->addWidget(loadedStatusLabel, 0, 0, 1, -1);
    grid->addWidget(lengthOneWarning, 1, 0, 1, 2);
    grid->addWidget(selectLengthOneButton, 1, 2);
    grid->addWidget(tree, 2, 0, 1, -1);
    setLayout(grid);
}

void SeriesDisplayPage::initializePage()
{
    PhotoSeriesVec const seriesVec = field("dividedSeries").value<PhotoSeriesVec>();
    std::size_t photoCount = 0;
    std::size_t lengthOneSeries = 0;

    for (PhotoSeries const& series : seriesVec)
    {
        photoCount += series.size();
        if (series.size() == 1)
            ++lengthOneSeries;

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
    loadedStatusLabel->setText(tr("Loaded %1 photos into %2 series").arg(photoCount).arg(seriesVec.size()));

    if (lengthOneSeries > 0)
    {
        selectLengthOneButton->show();
        lengthOneWarning->show();
        lengthOneWarning->setText(tr("%1 series with only one photo %2 been disabled").arg(lengthOneSeries)
                                                                                      .arg(lengthOneSeries == 1 ? "has" : "have"));
    }
}

void SeriesDisplayPage::cleanupPage()
{
    QTreeWidgetItem *item;
    while ((item = tree->takeTopLevelItem(0)))
        delete item;
}

void SeriesDisplayPage::selectBackSeriesWithOnePhoto()
{
    for (int i = 0; i < tree->topLevelItemCount(); ++i)
        if (tree->topLevelItem(i)->type() == TREEITEM_LENGTHONESERIES)
            tree->topLevelItem(i)->setCheckState(0, Qt::Checked);

    lengthOneWarning->hide();
    selectLengthOneButton->hide();
}

}} // namespace phobos::importwiz
