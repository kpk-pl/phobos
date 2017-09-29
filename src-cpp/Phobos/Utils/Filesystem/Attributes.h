#ifndef UTILS_FILESYSTEM_ATTRIBUTES_H
#define UTILS_FILESYSTEM_ATTRIBUTES_H

#include <string>

namespace phobos { namespace utils { namespace fs {

// TODO: verify QFileInfo::lastModified or QFileInfo::created as it supposed to return the same
unsigned lastModificationTime(std::string const& fileName);

}}} // namespace phobos::utils::fs

#endif // UTILS_FILESYSTEM_ATTRIBUTES_H
