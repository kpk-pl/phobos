#ifndef IMPORTWIZARD_IMPORTWIZARD_H
#define IMPORTWIZARD_IMPORTWIZARD_H

#include <QWizard>
#include "ImportWizard/Types.h"

namespace phobos { namespace importwiz {
class DivisionMethodPage;
class SeriesDisplayPage;

// TODO: Enable importing not as a series but as a collection of photos
// need to mark that series that it should not aggregate series metrics
// also allseriesview might display some icon on the left where the ord numbers are
// by default use hardcoded 5 or 6 photos per pseudo-series
class ImportWizard : public QWizard
{
  Q_OBJECT
public:
  ImportWizard(QWidget *parent = nullptr);

  PhotoSeriesVec selectedSeries() const;

private:
  DivisionMethodPage *divisionPage;
  SeriesDisplayPage *displayPage;
};

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_IMPORTWIZARD_H
