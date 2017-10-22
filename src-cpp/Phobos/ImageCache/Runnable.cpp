#include "ImageCache/Runnable.h"

namespace phobos { namespace icache {

Runnable::Runnable() : _uuid(QUuid::createUuid())
{}

void Runnable::run()
{
  try
  {
    runImpl();
  }
  catch(...)
  {
    emit signal.interrupted(id());
  }
}

}} // namespace phobos::icache
