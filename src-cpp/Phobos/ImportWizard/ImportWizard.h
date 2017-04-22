#ifndef IMPORTWIZARD_IMPORTWIZARD_H
#define IMPORTWIZARD_IMPORTWIZARD_H

#include <QWizard>
#include "ImportWizard/Types.h"

namespace phobos { namespace importwiz {
class DivisionMethodPage;

class ImportWizard : public QWizard
{
    Q_OBJECT
public:
    ImportWizard(QWidget *parent = nullptr);

    std::vector<PhotoSeries> const& loadedSeries() const;
    void accept() override;

private:
    DivisionMethodPage *divisionPage;
};

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_IMPORTWIZARD_H
