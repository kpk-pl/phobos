#include "ImageCache/PriorityThreadPool.h"
#include <cassert>

namespace phobos { namespace icache {

PriorityThreadPool::PriorityThreadPool()
{}

void PriorityThreadPool::start(RunnablePtr && task, std::size_t const priority)
{
  insertTask(std::move(task), priority);
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
  if (std::find_if(queue.begin(), queue.end(), IdEqual(task->id())) != queue.end())
    return false;

  if (runningTasks.find(task->id()) != runningTasks.end())
    return false;

  auto const where = std::upper_bound(queue.begin(), queue.end(), priority,
        [](std::size_t const prio, PriorityTask const& p){ return prio < p.first; });
  queue.emplace(where, priority, std::move(task));

  return true;
}

void PriorityThreadPool::updatePool()
{
  if (static_cast<int>(runningTasks.size()) >= pool.maxThreadCount())
    return;

  if (queue.empty())
    return;

  PriorityTask &pTask = queue.front();
  QObject::connect(&pTask.second->signal, &RunnableSignals::finished, this, &PriorityThreadPool::taskFinished);
  runningTasks.insert(pTask.first);
  pool.start(pTask.second.release());
  queue.erase(queue.begin());
}

void PriorityThreadPool::taskFinished(Runnable::Id id)
{
  auto const it = runningTasks.find(id);
  assert(it != runningTasks.end());
  runningTasks.erase(it);

  updatePool();
}

}} // namespace phobos::icache
