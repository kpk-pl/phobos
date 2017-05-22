#include <cassert>
#include <easylogging++.h>
#include "Future.h"
#include "Utils/Streaming.h"

namespace phobos { namespace icache {

Future::Future(CreateMode mode, QImage const& image)
{
    switch(mode)
    {
    case CreateMode::Ready:
        LOG(DEBUG) << "F" << utils::stream::ObjId()(this) << " : creating ready";
        readyImage = image;
        break;
    case CreateMode::Preload:
        LOG(DEBUG) << "F" << utils::stream::ObjId()(this) << " : creating preload";
        preloadImage = image;
        break;
    default:
        assert(false);
    };
}

FuturePtr Future::createReady(QImage const& readyImage)
{
    return std::make_shared<Future>(CreateMode::Ready, readyImage);
}

FuturePtr Future::createPreload(QImage const& preloadImage)
{
    return std::make_shared<Future>(CreateMode::Preload, preloadImage);
}

void Future::setImage(QImage image)
{
    LOG(DEBUG) << "F" << utils::stream::ObjId()(this) << " : setting image";
    readyImage = image;
    emit imageReady(readyImage);
}

}} // namespace phobos::icache
