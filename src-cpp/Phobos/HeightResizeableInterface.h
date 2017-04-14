#ifndef HEIGHTRESIZEABLEINTERFACE_H
#define HEIGHTRESIZEABLEINTERFACE_H

namespace phobos {

class HeightResizeableInterface
{
public:
    virtual bool hasWidthForHeight() const = 0;
    virtual int widthForHeight(int const) const = 0;
};

} // namespace phobos

#endif // HEIGHTRESIZEABLEINTERFACE_H
