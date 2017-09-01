#ifndef IMAGECACHE_PRIORITY_THREAD_POOL_H_
#define IMAGECACHE_PRIORITY_THREAD_POOL_H_

#include "ImageCache/Runnable.h"
#include <QThreadPool>
#include <QObject>
#include <vector>
#include <set>

namespace phobos { namespace icache {

class PriorityThreadPool : public QObject
{
  Q_OBJECT

public:
  explicit PriorityThreadPool();

  void start(RunnablePtr && task, std::size_t const priority);
  void cancel(Runnable::Id const& id);

private slots:
  void taskFinished(Runnable::Id taskId);

private:
  bool insertTask(RunnablePtr && task, std::size_t const priority);
  void updatePool();

  using PriorityTask = std::pair<std::size_t, RunnablePtr>;

  std::set<std::size_t> runningTasks;
  std::vector<PriorityTask> queue;

  QThreadPool pool;
};

}}

#endif // IMAGECACHE_PRIORITY_THREAD_POOL_H_
