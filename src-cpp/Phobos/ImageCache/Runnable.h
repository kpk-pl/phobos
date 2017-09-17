#ifndef IMAGECACHE_RUNNABLE_H_
#define IMAGECACHE_RUNNABLE_H_

#include <QRunnable>
#include <QMetaType>
#include <QObject>
#include <QUuid>
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
  using UniqueId = QUuid;

  explicit Runnable() : _uuid(QUuid::createUuid())
  {}

  virtual ~Runnable() = default;

  using Id = std::size_t;
  virtual Id id() const = 0;
  UniqueId const& uuid() const { return _uuid; }

  RunnableSignals signal;

private:
  UniqueId const _uuid;
};

using RunnablePtr = std::unique_ptr<Runnable>;

}} // namespace phobos::icache

Q_DECLARE_METATYPE(std::size_t)

#endif // IMAGECACHE_RUNNABLE_H_
