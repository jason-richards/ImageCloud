#include <list>
#include <mutex>
#include <condition_variable>
#include "workqueue.hpp"


class WorkQueue {
  std::mutex m_Mutex;
  std::condition_variable m_Conditional;
  std::list<IPlugInPtr> m_Work;

public:

  WorkQueue() {}
 ~WorkQueue() {}


  void
  AddWork(
    IPlugInPtr& work
  ) {
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_Work.push_back(work);
    m_Conditional.notify_one();
  }


  IPlugInPtr
  GetWork() {
    std::unique_lock<std::mutex> lock(m_Mutex);
    if (m_Work.size()==0) {
      m_Conditional.wait(lock);
      if (m_Work.size()==0) {
        return nullptr;
      }
    }

    auto work = m_Work.front();
    m_Work.pop_front();
    return work;
  }


  size_t
  GetSize() {
    std::unique_lock<std::mutex> lock(m_Mutex);
    return m_Work.size();
  }

  void
  NotifyAll() {
    m_Conditional.notify_all();
  }
};


/* Create a new WorkQueue Pointer.
 *
 * @result WorkQueuePtr - Pointer to a new Work Queue
 */
WorkQueuePtr
CreateWorkQ() {
  return std::make_shared<WorkQueue>();
}


/* Add a task from the Work Queue.
 * @param WorkQueuePtr - Pointer to the work queue.
 * @param IPlugInPtr - Pointer to the task to run. 
 * @result - Unique identifier string generated for the task.
 */
void
AddWork(
  WorkQueuePtr wQ,
  IPlugInPtr work
) {
  wQ->AddWork(work);
}


/* Get a task from the Work Queue.
 * @param WorkQueuePtr - Pointer to the work queue.
 * @result - A pointer to the work returned.
 */
IPlugInPtr
GetWork(
  WorkQueuePtr wQ
) {
  return wQ->GetWork();
}


/* Obtain number of tasks in Work Queue.
 * @param WorkQueuePtr - Pointer to the work queue.
 * @result - Number of tasks in Work Queue
 */
size_t
GetSize(
  WorkQueuePtr wQ
) {
  return wQ->GetSize();
}


/* Called before shut down to release all listeners.
 */
void
NotifyAll(
  WorkQueuePtr wQ
) {
  wQ->NotifyAll();
}

