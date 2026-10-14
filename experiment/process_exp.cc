/**
 * @file process_exp.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-05
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <unistd.h>
#include <map>

struct ProcessId {
  pid_t process_id;
};

class Process {
 public:
  static Process const& GetCurrentProcess() {}

 private:
  ProcessId process_id_;
  std::map<ProcessId, Process> child_process_;
};
