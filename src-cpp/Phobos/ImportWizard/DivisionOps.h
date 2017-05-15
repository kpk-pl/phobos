#ifndef IMPORTWIZARD_DIVISIONOPS_H
#define IMPORTWIZARD_DIVISIONOPS_H

#include <string>
#include <vector>
#include <QStringList>
#include "ImportWizard/Types.h"

namespace phobos { namespace importwiz {

// TODO: install and use EXIV2 lib for reading metadata

PhotoSeriesVec divideToSeriesNoop(QStringList const& photos);
PhotoSeriesVec divideToSeriesWithEqualSize(QStringList const& photos, std::size_t const photosInSeries);
PhotoSeriesVec divideToSeriesOnMetadata(QStringList const& photos);

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_DIVISIONOPS_H
