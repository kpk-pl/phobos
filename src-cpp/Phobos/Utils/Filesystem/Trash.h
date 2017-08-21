#ifndef UTILS_FILESYSTEM_TRASH_H
#define UTILS_FILESYSTEM_TRASH_H

#include <QString>

namespace phobos { namespace utils { namespace fs {

bool trashAvailable();
bool moveToTrash(QString const& file);

}}} // namespace phobos::utils::fs

#endif // UTILS_FILESYSTEM_TRASH_H
