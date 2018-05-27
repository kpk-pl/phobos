#ifndef IMAGECACHE_PRIORITY_THREAD_POOL_H_
#define IMAGECACHE_PRIORITY_THREAD_POOL_H_

#include "ImageCache/Runnable.h"
#include "ImageCache/Priority.h"
#include "ImageCache/TransactionFwd.h"
#include <QThreadPool>
#include <QObject>
#include <list>
#include <set>

namespace phobos { namespace icache {

class PriorityThreadPool : public QObject
{
  Q_OBJECT

public:
  explicit PriorityThreadPool() = default;

  void start(RunnablePtr && task, ConstTransactionPtr const& transaction);
  void cancel(Runnable::UniqueId const& taskUniqueId);

private slots:
  void taskFinished(Runnable::Id taskId);
  void taskInterrupted(Runnable::Id taskId);

private:
  void insertTask(RunnablePtr && task, Priority const& priority, bool const background);
  void handleFinished(Runnable::Id taskId);
  void updatePool();

  struct PriorityTask
  {
    struct IdEqual;
    struct UniqueIdEqual;

    PriorityTask(Priority const& priority, bool const background, RunnablePtr && task) :
      priority(priority), background(background), task(std::move(task))
    {}

    Priority priority;
    bool background;
    RunnablePtr task;

    // lower tasks execute first
    bool operator<(PriorityTask const& rhs) const;
  };

  using QueueType = std::list<PriorityTask>;

  QueueType::iterator findNextTask();

  std::set<std::size_t> runningTasks;
  QueueType queue;

  QThreadPool pool;
};

}}

#endif // IMAGECACHE_PRIORITY_THREAD_POOL_H_
