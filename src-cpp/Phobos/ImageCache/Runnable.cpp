#include "ImageCache/Runnable.h"
#include <easylogging++.h>

namespace phobos { namespace icache {

Runnable::Runnable() : _uuid(QUuid::createUuid())
{}

void Runnable::run()
{
  try
  {
    runImpl();
  }
  catch(std::runtime_error const& e)
  {
    LOG(ERROR) << "Runnable " << id() << " crashed (" << e.what() << ")";
    emit signal.interrupted(id());
  }
  catch(...)
  {
    LOG(ERROR) << "Runnable " << id() << " crashed";
    emit signal.interrupted(id());
  }
}

}} // namespace phobos::icache
