#include "ImageCache/PriorityThreadPool.h"
#include <easylogging++.h>
#include <cassert>

namespace phobos { namespace icache {

PriorityThreadPool::PriorityThreadPool()
{}

void PriorityThreadPool::start(RunnablePtr && task, std::size_t const priority)
{
  bool const inserted = insertTask(std::move(task), priority);

  if (inserted)
    updatePool();
}

namespace {
  struct IdEqual
  {
    IdEqual(Runnable::Id const id)
      : id(id)
    {}

    bool operator()(std::pair<std::size_t, RunnablePtr> const& p) const
    {
      return p.second->id() == id;
    }
    Runnable::Id const id;
  };
} // unnamed namespace

bool PriorityThreadPool::insertTask(RunnablePtr && task, std::size_t const priority)
{
  LOG(DEBUG) << "Attempt to insert task " << task->id() << " with priority " << priority;

  if (runningTasks.find(task->id()) != runningTasks.end())
  {
    LOG(DEBUG) << "Task already running";
    return false;
  }

  auto const scheduledId = std::find_if(queue.begin(), queue.end(), IdEqual(task->id()));
  if (scheduledId != queue.end())
  {
    LOG(DEBUG) << "Task already scheduled with priority " << scheduledId->first;
    if (scheduledId->first <= priority)
      return false;

    LOG(DEBUG) << "Raising priority";
    task = std::move(scheduledId->second);
    queue.erase(scheduledId);
  }

  LOG(DEBUG) << "Inserting new task to queue";
  auto const where = std::upper_bound(queue.begin(), queue.end(), priority,
        [](std::size_t const prio, PriorityTask const& p){ return prio < p.first; });
  queue.emplace(where, priority, std::move(task));

  return true;
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

  auto taskToRun = std::move(queue.front().second);
  queue.erase(queue.begin());

  QObject::connect(&taskToRun->signal, &RunnableSignals::finished, this, &PriorityThreadPool::taskFinished);
  runningTasks.insert(taskToRun->id());
  LOG(DEBUG) << "Starting task " << taskToRun->id();
  pool.start(taskToRun.release());
}

void PriorityThreadPool::taskFinished(Runnable::Id id)
{
  LOG(DEBUG) << "Finished task " << id;
  auto const it = runningTasks.find(id);
  assert(it != runningTasks.end());
  runningTasks.erase(it);

  updatePool();
}

}} // namespace phobos::icache
