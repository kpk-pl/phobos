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
        [](std::size_t const prio, PriorityTask const& p){ return prio > p.priority; });
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

  auto taskToRunIt = findNextTask();
  if (taskToRunIt == queue.end())
  {
    LOG(DEBUG) << "No more tasks to run in queue";
    return;
  }

  auto taskToRun = std::move(taskToRunIt->task);
  queue.erase(taskToRunIt);

  QObject::connect(&taskToRun->signal, &RunnableSignals::finished, this, &PriorityThreadPool::taskFinished);
  QObject::connect(&taskToRun->signal, &RunnableSignals::interrupted, this, &PriorityThreadPool::taskInterrupted);
  runningTasks.insert(taskToRun->id());
  LOG(DEBUG) << "Starting task " << taskToRun->id();
  pool.start(taskToRun.release());
}

PriorityThreadPool::QueueType::iterator PriorityThreadPool::findNextTask()
{
  for (auto it = queue.begin(); it != queue.end(); ++it)
    if (runningTasks.find(it->task->id()) == runningTasks.end())
      return it;

  return queue.end();
}

void PriorityThreadPool::taskFinished(Runnable::Id taskId)
{
  LOG(DEBUG) << "Finished task " << taskId;
  handleFinished(taskId);
}

void PriorityThreadPool::taskInterrupted(Runnable::Id taskId)
{
  // TODO: Handle crashed task, maybe restart?
  LOG(WARNING) << "Task crashed! (" << taskId << ")";
  handleFinished(taskId);
}

void PriorityThreadPool::handleFinished(Runnable::Id taskId)
{
  auto const it = runningTasks.find(taskId);
  assert(it != runningTasks.end());
  runningTasks.erase(it);

  queue.erase(std::remove_if(queue.begin(), queue.end(), PriorityTask::IdEqual{taskId}), queue.end());

  updatePool();
}

}} // namespace phobos::icache
