#include "ImageCache/PriorityThreadPool.h"
#include "ImageCache/Transaction.h"
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

void PriorityThreadPool::start(RunnablePtr && task, ConstTransactionPtr const& transaction)
{
  insertTask(std::move(task), transaction->priority, transaction->imageSize == ImageSize::Thumbnail);
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
  if (priority.proactiveGeneration != rhs.priority.proactiveGeneration) // first come the lowest proactive generation
    return priority.proactiveGeneration < rhs.priority.proactiveGeneration;

  if (priority.timestamp != rhs.priority.timestamp) // then order timestamps
    if (background)
      return priority.timestamp < rhs.priority.timestamp; // for background it is first-come-first-serve
    else
      return priority.timestamp > rhs.priority.timestamp; // for organic, newest first

  return task < rhs.task;
}

void PriorityThreadPool::insertTask(RunnablePtr && task, Priority const& priority, bool const background)
{
  LOG(DEBUG) << "[Cache] Inserting task " << task->id() << " with priority " << priority.toString();

  PriorityTask pt{priority, background, std::move(task)};
  auto const where = std::upper_bound(queue.begin(), queue.end(), std::cref(pt),
      [](PriorityTask const& newTask, PriorityTask const& task){ return newTask < task; });

  queue.insert(where, std::move(pt));
}

void PriorityThreadPool::updatePool()
{
  LOG(DEBUG) << "[Cache] Updating pool. Tasks in queue: " << queue.size() << " running: " << runningTasks.size();

  if (static_cast<int>(runningTasks.size()) >= pool.maxThreadCount())
  {
    LOG(DEBUG) << "[Cache] No free threads to run more tasks";
    return;
  }

  auto taskToRunIt = findNextTask();
  if (taskToRunIt == queue.end())
  {
    LOG(DEBUG) << "[Cache] No more tasks to run in queue";
    return;
  }

  RunnablePtr taskToRun;
  std::swap(taskToRun, taskToRunIt->task);
  queue.erase(taskToRunIt);

  QObject::connect(&taskToRun->signal, &RunnableSignals::finished, this, &PriorityThreadPool::taskFinished);
  QObject::connect(&taskToRun->signal, &RunnableSignals::interrupted, this, &PriorityThreadPool::taskInterrupted);
  runningTasks.insert(taskToRun->id());
  LOG(DEBUG) << "[Cache] Starting task " << taskToRun->id();
  pool.start(taskToRun.release());
}


// TODO: Don't start loading proactively when there is no more cache space left for given generation. Handle persistency flag
// Remove such tasks from queue
PriorityThreadPool::QueueType::iterator PriorityThreadPool::findNextTask()
{
  for (auto it = queue.begin(); it != queue.end(); ++it)
    if (runningTasks.find(it->task->id()) == runningTasks.end())
      return it;

  return queue.end();
}

void PriorityThreadPool::taskFinished(Runnable::Id taskId)
{
  LOG(DEBUG) << "[Cache] Finished task " << taskId;
  handleFinished(taskId);
}

void PriorityThreadPool::taskInterrupted(Runnable::Id taskId)
{
  LOG(WARNING) << "[Cache] Task crashed! (" << taskId << ")";
  handleFinished(taskId);
}

void PriorityThreadPool::handleFinished(Runnable::Id taskId)
{
  auto const it = runningTasks.find(taskId);
  assert(it != runningTasks.end());
  runningTasks.erase(it);

  auto const originalSize = queue.size();
  queue.erase(std::remove_if(queue.begin(), queue.end(), PriorityTask::IdEqual{taskId}), queue.end());
  if (queue.size() != originalSize)
    LOG(DEBUG) << "[Cache] Removed " << originalSize - queue.size() << " tasks from queue";

  updatePool();
}

}} // namespace phobos::icache
