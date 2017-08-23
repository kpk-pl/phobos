#ifndef IMPORTWIZARD_DIVISIONOPS_H
#define IMPORTWIZARD_DIVISIONOPS_H

#include <vector>
#include "ImportWizard/Types.h"

namespace phobos { namespace importwiz {

PhotoSeriesVec divideToSeriesNoop(std::vector<Photo> && photos);
PhotoSeriesVec divideToSeriesWithEqualSize(std::vector<Photo> && photos, std::size_t const photosInSeries);
PhotoSeriesVec divideToSeriesOnMetadata(std::vector<Photo> && photos);

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_DIVISIONOPS_H
