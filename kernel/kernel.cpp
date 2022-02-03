#include <yaml-cpp/yaml.h>
#include <uuid/uuid.h>
#include <zmq.h>
#include <mutex>
#include <thread>

#include "kernel.hpp"
#include "plugin-factory.hpp"
#include "workqueue.hpp"
#include "aixlog.hpp"

namespace KERNEL {



//////////////////////////////////////////////////////
//  Stop Token
//////////////////////////////////////////////////////
class StopToken {
  std::mutex m_Done;
public:

  static StopTokenPtr
  Create () {
    return std::make_shared<StopToken>();
  }

  void Start()         { m_Done.lock();   }
  void Stop()          { m_Done.unlock(); }
  bool StopRequested() { return m_Done.try_lock() ? true : false; }
};



/* Create a new StopToken Pointer.
 *
 * @result StopTokenPtr - Pointer to a new StopToken object.
 */
StopTokenPtr
CreateStopToken() {
  return StopToken::Create();
}



/* Request an asynchronous stop.
 *
 * @param stopper - Pointer to a StopToken object;
 */
void
RequestStop(
  StopTokenPtr stopper
) {
  stopper->Stop();
}



//////////////////////////////////////////////////////
//  Kernel Context
//////////////////////////////////////////////////////
class Context {
  using ThreadPtr = std::shared_ptr<std::jthread>;
  std::list<ThreadPtr> m_WorkerThreads;
  std::list<StopTokenPtr> m_StopTokens;

public:

  static ContextPtr
  Create (
    const std::string& configYAML
  ) {
    return std::make_shared<Context>(configYAML);
  }

  Context(const std::string& configYAML) {
    AixLog::Log::init<AixLog::SinkCout>(AixLog::Severity::debug);
    YAML::Node config = YAML::LoadFile(configYAML);
    m_StoragePath     = config["storage"].as<std::string>();
    m_ZMQSocketString = config["socket"].as<std::string>();
    m_ThreadCount     = config["threads"] ? config["threads"].as<int>() : 2;
    m_TimeoutMs       = config["timeout"] ? config["timeout"].as<int>() : 1000;
    m_ZMQContext      = zmq_ctx_new();
    m_ZMQResponder    = zmq_socket(m_ZMQContext, ZMQ_REP);
    m_WorkQueue       = CreateWorkQ();
    m_Running         = false;
    m_ZMQEndPoint.resize(2048, 0);

    LOG(INFO) << "Starting Kernel Process"  << std::endl;
    LOG(INFO) << "Threads: "                << m_ThreadCount      << std::endl;
    LOG(INFO) << "ZMQ Socket: "             << m_ZMQSocketString  << std::endl;
    LOG(INFO) << "Timeout (ms): "           << m_TimeoutMs        << std::endl;

    for (int i = 0; i < m_ThreadCount; i++) {
      LOG(INFO) << "Creating Worker Thread: " << i << std::endl;
      m_WorkerThreads.push_back(NewWorker());
    }
  }

 ~Context() {
    if (m_Running) {
      char * endpoint     = m_ZMQEndPoint.data();
      size_t endpoint_len = m_ZMQEndPoint.size()-1;
      if (!zmq_getsockopt(m_ZMQResponder, ZMQ_LAST_ENDPOINT, endpoint, &endpoint_len) && 
          endpoint && *endpoint && endpoint_len
      ) {
        zmq_unbind(m_ZMQResponder, endpoint);
      }
    }
    zmq_close(m_ZMQResponder);
    zmq_ctx_destroy(m_ZMQContext);
  }

  ThreadPtr
  NewWorker() {
    int threadId = m_StopTokens.size();
    StopTokenPtr stp = CreateStopToken();
    m_StopTokens.push_back(stp);
    return std::make_shared<std::jthread>(
      [&](StopTokenPtr stopper, int threadId) {
        stopper->Start();
        while (!stopper->StopRequested()) {
          auto workTuple = GetWork(m_WorkQueue);
          auto UUID = std::get<0>(workTuple);
          auto work = std::get<1>(workTuple);
          if (!work) {
            continue;
          }
          LOG(INFO) << "Worker Thread " << threadId << " started work ID: " << UUID << std::endl;
          work->Start();
          LOG(INFO) << "Worker Thread " << threadId << " finished work ID: " << UUID << std::endl;
        }
      }, stp, threadId
    );
  }

  bool
  Start(
    StopTokenPtr stopper
  ) {
    if (zmq_bind(m_ZMQResponder, m_ZMQSocketString.c_str()) ||
        zmq_setsockopt(m_ZMQResponder, ZMQ_RCVTIMEO, &m_TimeoutMs, sizeof(m_TimeoutMs))
    ) {
      return false;
    }

    m_Running = true;

    stopper->Start();
    while (!stopper->StopRequested()) {
      std::vector<uint8_t> request;

      if (Receive<std::vector<uint8_t>>(request)) {
        std::string errors;
        IPlugInPtr plugin = m_PlugIns.GetPlugIn(request, errors);
        if (plugin) {
          std::string uuid = AddWork(m_WorkQueue, plugin);
          zmq_send(m_ZMQResponder, uuid.data(), uuid.size(), ZMQ_DONTWAIT);
        } else {
          zmq_send(m_ZMQResponder, errors.data(), errors.size(), ZMQ_DONTWAIT);
        }
      } 
    }

    for (auto stp : m_StopTokens) {
      stp->Stop();
    }

    NotifyAll(m_WorkQueue);

    for (auto worker : m_WorkerThreads) {
      worker->join();
    }

    return true;
  }

  void
  Status(
    std::string& statusString
  ) {
    statusString = m_Running ? "running" : "stopped";
  }

private:

  template <typename T>
  bool
  Receive(
    T& request
  ) {
    zmq_msg_t part;
    do {
      if (zmq_msg_init(&part) || zmq_recvmsg(m_ZMQResponder, &part, 0) < 0) {
        return false;
      }

      size_t size = zmq_msg_size(&part);
      void * data = zmq_msg_data(&part);

      size_t sz = request.size();
      request.resize(sz + size);
      std::memcpy(request.data() + sz, data, size);

    } while (zmq_msg_more(&part) == 1);

    return true;
  }

  std::string   m_StoragePath;
  std::string   m_ZMQSocketString;
  std::string   m_ZMQEndPoint;

  void *         m_ZMQContext;
  void *         m_ZMQResponder;

  bool          m_Running;

  PlugInFactory m_PlugIns;
  int           m_ThreadCount;
  int           m_TimeoutMs;

  WorkQueuePtr  m_WorkQueue;
};



/* Create a new Kernel Context Pointer.
 *
 * @param configFile   - Name of YAML Configuration file.
 * @result ContextPtr - Pointer to a new Kernel Context
 */
ContextPtr
CreateContext (
  const std::string&    configYAML
) {
  return Context::Create(configYAML);
}



/* Start the Kernel.
 *
 * @param context - Pointer to Kernel Context
 * @param stopper - Pointer to a StopToken object for asynchronous control.
 * @result bool   - True if successfully started Kernel.
 */
bool
Start (
  ContextPtr  context,
  StopTokenPtr stopper
) {
  return context->Start(stopper);
}



/* Obtain status of the Kernel.
 *
 * @param context - Pointer to Kernel Context
 * @param string  - Return kernel status string
 */
void
Status (
  ContextPtr     context,
  std::string&   statusString
) {
  return context->Status(statusString);
}


} // namespace KERNEL

