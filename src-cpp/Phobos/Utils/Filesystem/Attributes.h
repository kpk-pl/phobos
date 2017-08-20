#ifndef UTILS_FILESYSTEM_ATTRIBUTES_H
#define UTILS_FILESYSTEM_ATTRIBUTES_H

#include <string>

namespace phobos { namespace utils { namespace fs {

unsigned lastModificationTime(std::string const& fileName);

}}} // namespace phobos::utils::fs

#endif // UTILS_FILESYSTEM_ATTRIBUTES_H