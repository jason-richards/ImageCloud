#include <algorithm>
#include <iomanip>
#include <iostream>
#include <ios>

#include "status-plugin.hpp"
#include "stat-probe.hpp"


Status::Status(
  const YAML::Node& config,
  const rapidjson::Document& request
) : IPlugIn(config, request) {}


bool
Status::Initialize(
  Responder responder
) {
  int pid = 0;
  int maxThreads = 0;
  uint64_t rss = 0;
  uint64_t vmsize = 0;
  uint64_t majfaults = 0;
  uint64_t minfaults = 0;

  System::Stat::ProbePtr SP = System::Stat::CreateProbe();
  System::Stat::GetPID(SP, pid);
  System::Stat::GetMaxThreads(SP, maxThreads);
  System::Stat::GetResidentSetSize(SP, rss);
  System::Stat::GetVirtualMemSize(SP, vmsize);
  System::Stat::GetMinorFaults(SP, minfaults);
  System::Stat::GetMajorFaults(SP, majfaults);

  auto jsonResponse = std::string("{\"status\" : \"OK\"");

  jsonResponse += ", \"pid\" : ";
  jsonResponse += std::to_string(pid);

  jsonResponse += ", \"max_threads\" : ";
  jsonResponse += std::to_string(maxThreads);

  jsonResponse += ", \"vsize\" : ";
  jsonResponse += std::to_string(vmsize);

  jsonResponse += ", \"rss\" : ";
  jsonResponse += std::to_string(rss);

  jsonResponse += ", \"minor_faults\" : ";
  jsonResponse += std::to_string(minfaults);

  jsonResponse += ", \"major_faults\" : ";
  jsonResponse += std::to_string(majfaults);

  jsonResponse += "}";

  return responder(jsonResponse.data(), jsonResponse.size(), false);
}

