#ifndef IMAGECACHE_RUNNABLE_H_
#define IMAGECACHE_RUNNABLE_H_

#include <QRunnable>
#include <QObject>
#include <memory>

namespace phobos { namespace icache {

class RunnableSignals : public QObject
{
  Q_OBJECT

signals:
  void finished(std::size_t id);
};

class Runnable : public QRunnable
{
public:
  using QRunnable::QRunnable;
  virtual ~Runnable() = default;

  using Id = std::size_t;
  virtual Id id() const = 0;

  RunnableSignals signal;
};

using RunnablePtr = std::unique_ptr<Runnable>;

}} // namespace phobos::icache

#endif // IMAGECACHE_RUNNABLE_H_
