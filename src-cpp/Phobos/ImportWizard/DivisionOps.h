#ifndef IMPORTWIZARD_DIVISIONOPS_H
#define IMPORTWIZARD_DIVISIONOPS_H

#include <string>
#include <vector>
#include <QStringList>
#include "ImportWizard/Types.h"

namespace phobos { namespace importwiz {

std::vector<PhotoSeries> divideToSeriesWithEqualSize(QStringList const& photos, std::size_t const photosInSeries);
std::vector<PhotoSeries> divideToSeriesOnMetadata(QStringList const& photos);

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_DIVISIONOPS_H
