#include "stat-probe.hpp"

#include <iostream>
#include <fstream>
#include <string>

namespace System {
namespace Stat {


class Probe {
public:
  std::string pid, comm, state, ppid, pgrp, session, tty_nr, tpgid, flags, minfault, cminfault, majfault, cmajfault,
              utime, stime, cutime, cstime, priority, nice, numthreads, itrealvalue, startime, vsize, rss, rsslimit;
  Probe() {
    std::ifstream stat("/proc/self/stat", std::ios_base::in);

    stat >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags >> minfault >> cminfault 
         >> majfault >> cmajfault >> utime >> stime >> cutime >> cstime >> priority >> nice >> numthreads
         >> itrealvalue >> startime >> vsize >> rss >> rsslimit;

    stat.close();
  }

};


/* Create a new Proc Stat probe.
 *
 * @result ProbePtr - Proc Stat probe pointer.
 */
ProbePtr
CreateProbe(
) {
  return std::make_shared<Probe>();
}


/* Obtain the process pid.
 *
 * @param ProbePtr - Proc Stat probe pointer.
 * @param int& - Process ID
 */
void
GetPID(
  ProbePtr SP,
  int& pid
) {
  pid = std::stoi(SP->pid);
}


/* Obtain the maximum number of threads for the process.
 *
 * @param ProbePtr - Proc Stat probe pointer.
 * @param int& - Max process threads.
 */
void
GetMaxThreads(
  ProbePtr SP,
  int& nThreads
) {
  nThreads = std::stoi(SP->numthreads);
}


/* Obtain the process resident set size; number of pages resident in real memory.
 *
 * @param ProbePtr - Proc Stat probe pointer.
 * @param uint64_t& - The process rss.
 */
void
GetResidentSetSize(
  ProbePtr SP,
  uint64_t& rss
) {
  rss = std::stoull(SP->rss);
}


/* Obtain the process virtual memory size.
 *
 * @param ProbePtr - Proc Stat probe pointer.
 * @param uint64_t& - The process virtual memory size in bytes.
 */
void
GetVirtualMemSize(
  ProbePtr SP,
  uint64_t& vmsize
) {
  vmsize = std::stoull(SP->vsize);
}


/* Obtain the number of minor faults.
 *
 * @param ProbePtr - Proc Stat probe pointer.
 * @param uint64_t& - The process minor faults.
 */
void
GetMinorFaults(
  ProbePtr SP,
  uint64_t& minfaults
) {
  minfaults = std::stoull(SP->minfault);
}


/* Obtain the number of major faults.
 *
 * @param ProbePtr - Proc Stat probe pointer.
 * @param uint64_t& - The process major faults.
 */
void
GetMajorFaults(
  ProbePtr SP,
  uint64_t& majfaults
) {
  majfaults = std::stoull(SP->majfault);
}


} // namespace Stat 
} // namespace System

