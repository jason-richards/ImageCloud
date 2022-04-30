#pragma once

#include <memory>
#include <string>
#include <vector>

namespace System {
namespace Stat {

class Probe;
using ProbePtr = std::shared_ptr<Probe>;


/* Create a new Proc Stat probe.
 *
 * @result ProbePtr - Proc Stat probe pointer.
 */
ProbePtr
CreateProbe();


/* Obtain the process pid.
 *
 * @param ProbePtr - Proc Stat probe pointer.
 * @param int& - Process ID
 */
void
GetPID(
  ProbePtr,
  int&
);


/* Obtain the maximum number of threads for the process.
 *
 * @param ProbePtr - Proc Stat probe pointer.
 * @param int& - Max process threads.
 */
void
GetMaxThreads(
  ProbePtr,
  int&
);


/* Obtain the process resident set size; number of pages resident in real memory.
 *
 * @param ProbePtr - Proc Stat probe pointer.
 * @param uint64_t& - The process rss.
 */
void
GetResidentSetSize(
  ProbePtr,
  uint64_t&
);


/* Obtain the process virtual memory size.
 *
 * @param ProbePtr - Proc Stat probe pointer.
 * @param uint64_t& - The process virtual memory size in bytes.
 */
void
GetVirtualMemSize(
  ProbePtr,
  uint64_t&
);


/* Obtain the number of minor faults.
 *
 * @param ProbePtr - Proc Stat probe pointer.
 * @param uint64_t& - The process minor faults.
 */
void
GetMinorFaults(
  ProbePtr,
  uint64_t&
);


/* Obtain the number of major faults.
 *
 * @param ProbePtr - Proc Stat probe pointer.
 * @param uint64_t& - The process major faults.
 */
void
GetMajorFaults(
  ProbePtr,
  uint64_t&
);


} // namespace Stat 
} // namespace System

