#ifndef UTILS_PORTABLE_H_
#define UTILS_PORTABLE_H_

#include <string>

namespace phobos { namespace utils {

bool portableDirectoryName(const std::string & name);
bool portableFileName(const std::string & name);

}} // namespace phobos::utils

#endif // UTILS_PORTABLE_H_
