#include <easylogging++.h>
#include "Future.h"
#include "Utils/Streaming.h"

namespace phobos { namespace icache {

Future::Future(QImage const& initialPreloadImage) :
    preloadImage(initialPreloadImage),
    preloadReady(false)
{
}

FuturePtr Future::create(QImage const& initialPreloadImage)
{
    return std::make_shared<Future>(initialPreloadImage);
}

void Future::setImage(QImage image)
{
    LOG(DEBUG) << "F" << utils::stream::ObjId{}(this) << " : setting image";

    readyImage = image;

    if (!preloadReady)
    {
       preloadImage = image; // TODO fixme scale to size based on config
       preloadReady = true;
    }

    emit changed();
}

void Future::voidImage()
{
    LOG(DEBUG) << "F" << utils::stream::ObjId{}(this) << " : voiding image";
    readyImage = QImage{};
}

}} // namespace phobos::icache
