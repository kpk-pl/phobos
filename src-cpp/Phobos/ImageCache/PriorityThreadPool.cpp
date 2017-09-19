#include "ImageCache/PriorityThreadPool.h"
#include <easylogging++.h>
#include <cassert>

namespace phobos { namespace icache {

struct PriorityThreadPool::PriorityTask::IdEqual
{
  IdEqual(Runnable::Id const id)
    : id(id)
  {}

  bool operator()(PriorityThreadPool::PriorityTask const& pTask) const
  {
    if (pTask.task)
      return pTask.task->id() == id;
    return false;
  }

  Runnable::Id const id;
};

struct PriorityThreadPool::PriorityTask::UniqueIdEqual
{
  UniqueIdEqual(Runnable::UniqueId const id)
    : id(id)
  {}

  bool operator()(PriorityThreadPool::PriorityTask const& pTask) const
  {
    if (pTask.task)
      return pTask.task->uuid() == id;
    return false;
  }

  Runnable::UniqueId const id;
};

PriorityThreadPool::PriorityThreadPool()
{}

void PriorityThreadPool::start(RunnablePtr && task, std::size_t const priority)
{
  insertTask(std::move(task), priority);
  updatePool();
}

void PriorityThreadPool::cancel(Runnable::UniqueId const& uniqueTaskId)
{
  auto const queueIt = std::find_if(queue.begin(), queue.end(), PriorityTask::UniqueIdEqual{uniqueTaskId});
  if (queueIt != queue.end())
    queue.erase(queueIt);
}

bool PriorityThreadPool::PriorityTask::operator<(PriorityTask const& rhs) const
{
  if (priority == rhs.priority)
    return task < rhs.task;
  return priority < rhs.priority;
}

void PriorityThreadPool::insertTask(RunnablePtr && task, std::size_t const priority)
{
  LOG(DEBUG) << "Inserting task " << task->id() << " with priority " << priority;

  auto const where = std::upper_bound(queue.begin(), queue.end(), priority,
        [](std::size_t const prio, PriorityTask const& p){ return prio < p.priority; });
  queue.emplace(where, priority, std::move(task));
}

void PriorityThreadPool::updatePool()
{
  LOG(DEBUG) << "Updating pool. Tasks in queue: " << queue.size() << " running: " << runningTasks.size();

  if (static_cast<int>(runningTasks.size()) >= pool.maxThreadCount())
  {
    LOG(DEBUG) << "No free threads to run more tasks";
    return;
  }

  if (queue.empty())
  {
    LOG(DEBUG) << "No more tasks to run in queue";
    return;
  }

  // TODO: BUG!!! need to check running tasks if already running task does not do the same
  // thing! if so then need to skip!
  auto taskToRun = std::move(queue.front().task);
  queue.erase(queue.begin());

  QObject::connect(&taskToRun->signal, &RunnableSignals::finished, this, &PriorityThreadPool::taskFinished);
  runningTasks.insert(taskToRun->id());
  LOG(DEBUG) << "Starting task " << taskToRun->id();
  pool.start(taskToRun.release());
}

void PriorityThreadPool::taskFinished(Runnable::Id id)
{
  LOG(DEBUG) << "Finished task " << id;

  // TODO: BUG!!! running task is a set of simple id's. not unique. if two tasks with the same id run at
  // at the same time, then first one clears set and second one fails to do so because of assertion
  auto const it = runningTasks.find(id);
  assert(it != runningTasks.end());
  runningTasks.erase(it);

  queue.erase(std::remove_if(queue.begin(), queue.end(), PriorityTask::IdEqual{id}), queue.end());

  updatePool();
}

}} // namespace phobos::icache
