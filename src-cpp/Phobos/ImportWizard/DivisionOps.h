#ifndef IMPORTWIZARD_DIVISIONOPS_H
#define IMPORTWIZARD_DIVISIONOPS_H

#include <vector>
#include "ImportWizard/Types.h"

namespace phobos { namespace importwiz {

PhotoSeriesVec divideToSeriesNoop(std::vector<Photo> const& photos);
PhotoSeriesVec divideToSeriesWithEqualSize(std::vector<Photo> const& photos, std::size_t const photosInSeries);
PhotoSeriesVec divideToSeriesOnMetadata(std::vector<Photo> const& photos);

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_DIVISIONOPS_H
