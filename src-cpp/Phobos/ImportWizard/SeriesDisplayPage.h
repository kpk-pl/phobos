#ifndef IMPORTWIZARD_SERIESDISPLAYPAGE_H
#define IMPORTWIZARD_SERIESDISPLAYPAGE_H

#include <QWizardPage>

class QTreeWidget;
class QLabel;
class QPushButton;

namespace phobos { namespace importwiz {

class SeriesDisplayPage : public QWizardPage
{
    Q_OBJECT

public:
    SeriesDisplayPage(QWidget* parent = nullptr);

protected:
    void initializePage() override;
    void cleanupPage() override;

private slots:
    void selectBackSeriesWithOnePhoto();

private:  
    QTreeWidget *tree;

    QLabel *loadedStatusLabel;
    QLabel *lengthOneWarning;
    QPushButton *selectLengthOneButton;
};

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_SERIESDISPLAYPAGE_H
