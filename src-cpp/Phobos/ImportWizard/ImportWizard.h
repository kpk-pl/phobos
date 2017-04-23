#ifndef IMPORTWIZARD_IMPORTWIZARD_H
#define IMPORTWIZARD_IMPORTWIZARD_H

#include <QWizard>
#include "ImportWizard/Types.h"

namespace phobos { namespace importwiz {
class DivisionMethodPage;
class SeriesDisplayPage;

class ImportWizard : public QWizard
{
    Q_OBJECT
public:
    ImportWizard(QWidget *parent = nullptr);

    PhotoSeriesVec selectedSeries() const;
    void accept() override;

private:
    DivisionMethodPage *divisionPage;
    SeriesDisplayPage *displayPage;
};

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_IMPORTWIZARD_H
