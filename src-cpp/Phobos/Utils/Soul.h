#ifndef UTILS_SOUL_H_
#define UTILS_SOUL_H_

#include <memory>

namespace phobos { namespace utils {

template<typename T>
class Soul
{
public:
  using ObjectPtr = T*;

  explicit Soul(ObjectPtr const object) :
    _lifetime(std::make_shared<ObjectPtr>(object))
  {}

  std::weak_ptr<ObjectPtr> lifetime() const
  {
    return _lifetime;
  };

private:
  std::shared_ptr<ObjectPtr> _lifetime;
};

}} // namespace phobos::utils

#endif // UTILS_SOUL_H_
