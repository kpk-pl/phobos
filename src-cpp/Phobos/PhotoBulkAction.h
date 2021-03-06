#ifndef PHOTO_BULK_ACTION_H_
#define PHOTO_BULK_ACTION_H_

#include <QMetaType>

namespace phobos {

enum class PhotoBulkAction
{
  SELECT_BEST,
  SELECT_ALL,
  INVERT,
  CLEAR
};

} // namespace phobos

Q_DECLARE_METATYPE(phobos::PhotoBulkAction)

#endif // PHOTO_BULK_ACTION_H_
