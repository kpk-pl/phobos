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
  void cancel(Runnable::UniqueId const& taskUniqueId);

private slots:
  void taskFinished(Runnable::Id taskId);

private:
  void insertTask(RunnablePtr && task, std::size_t const priority);
  void updatePool();

  struct PriorityTask
  {
    struct IdEqual;
    struct UniqueIdEqual;

    PriorityTask(std::size_t const priority, RunnablePtr && task) :
      priority(priority), task(std::move(task))
    {}

    std::size_t priority;
    RunnablePtr task;

    bool operator<(PriorityTask const& rhs) const;
  };

  std::set<std::size_t> runningTasks;
  std::vector<PriorityTask> queue;

  QThreadPool pool;
};

}}

#endif // IMAGECACHE_PRIORITY_THREAD_POOL_H_
