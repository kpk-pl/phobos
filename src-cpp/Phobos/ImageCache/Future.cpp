#include "Future.h"

namespace phobos { namespace icache {

Future::Future()
{
}

Future::Future(QImage const& readyImage) :
    readyImage(readyImage)
{
}

FuturePtr Future::create()
{
    return std::make_shared<Future>();
}

FuturePtr Future::create(QImage const& readyImage)
{
    return std::make_shared<Future>(readyImage);
}

void Future::setImage(QImage const& image)
{
    readyImage = image;
    emit imageReady(readyImage);
}

}} // namespace phobos::icache
