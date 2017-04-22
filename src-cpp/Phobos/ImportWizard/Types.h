#ifndef IMPORTWIZARD_TYPES_H
#define IMPORTWIZARD_TYPES_H

#include <string>
#include <boost/optional.hpp>

namespace phobos { namespace importwiz {

struct Photo
{
    std::string fileName;
    boost::optional<unsigned> lastModTime;
};

typedef std::vector<Photo> PhotoSeries;

}} // namespace phobos::importwiz

#endif // IMPORTWIZARD_TYPES_H
