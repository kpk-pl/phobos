#ifndef UTILS_FILESYSTEM_PORTABLE_H_
#define UTILS_FILESYSTEM_PORTABLE_H_

#include <string>

namespace phobos { namespace utils { namespace fs {

bool portableDirectoryName(const std::string & name);
bool portableFileName(const std::string & name);

}}} // namespace phobos::utils::fs

#endif // UTILS_FILESYSTEM_PORTABLE_H_
