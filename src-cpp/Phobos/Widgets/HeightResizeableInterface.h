#ifndef PHOBOS_WIDGETS_HEIGHTRESIZEABLEINTERFACE_H
#define PHOBOS_WIDGETS_HEIGHTRESIZEABLEINTERFACE_H

namespace phobos { namespace widgets {

class HeightResizeableInterface
{
public:
  virtual ~HeightResizeableInterface() = default;

  virtual bool hasWidthForHeight() const = 0;
  virtual int widthForHeight(int const) const = 0;
};

}} // namespace phobos::widgets

#endif // PHOBOS_WIDGETS_HEIGHTRESIZEABLEINTERFACE_H
