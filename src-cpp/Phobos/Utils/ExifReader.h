#ifndef UTILS_EXIFREADER_H
#define UTILS_EXIFREADER_H

#include <easyexif/exif.h>
#include <boost/optional.hpp>

namespace phobos { namespace utils {

boost::optional<easyexif::EXIFInfo> readExif(std::string const& fileName);

}} // namespace phobos::utils

#endif // UTILS_EXIFREADER_H
