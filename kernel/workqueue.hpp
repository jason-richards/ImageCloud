#pragma once

#include <memory>
#include <tuple>

#include "iplugin.hpp"


class WorkQueue;
using WorkQueuePtr = std::shared_ptr<WorkQueue>;


/* Create a new WorkQueue Pointer.
 *
 * @result WorkQueuePtr - Pointer to a new Work Queue
 */
WorkQueuePtr
CreateWorkQ();


/* Add a task from the Work Queue.
 * @param WorkQueuePtr - Pointer to the work queue.
 * @param IPlugInPtr - Pointer to the task to run. 
 * @result - Unique identifier string generated for the task.
 */
std::string
AddWork(
  WorkQueuePtr,
  IPlugInPtr
);


/* Get a task from the Work Queue.
 * @param WorkQueuePtr - Pointer to the work queue.
 * @result - A string uuid /pointer to the work tuple returned.
 */
std::tuple<std::string, IPlugInPtr>
GetWork(
  WorkQueuePtr
);


/* Obtain number of tasks in Work Queue.
 * @param WorkQueuePtr - Pointer to the work queue.
 * @result - Number of tasks in Work Queue
 */
size_t
GetSize(
  WorkQueuePtr
);


/* Called before shut down to release all listeners.
 */
void
NotifyAll(
  WorkQueuePtr
);

