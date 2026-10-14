/**
 * @file name.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef BASE_IPC_NAME_H_
#define BASE_IPC_NAME_H_

#include <limits.h>

#include <cstring>
#include <set>
#include <string>

namespace base {
class IPCName;
class IPCNameSpace;

class IPCName {
 public:
  explicit IPCName(std::string const& name);

  IPCNameSpace* GetNameSpace() const;

 private:
  std::string ipc_name_;
  IPCNameSpace* ipc_namespace_;
};

class IPCNameSpace {
 public:
  IPCNameSpace(std::string const& ipc_name_space);

 private:
  std::set<std::string> ipc_names_;
};

std::string PosixIPCName(std::string const& name) {
  if (name.empty())
    return {};
  char const* px_ipc_name_env = getenv("PX_IPC_NAME");
  std::string dir = !px_ipc_name_env ? "" : px_ipc_name_env;
  if (dir.empty()) {
#ifdef POSIX_IPC_PREFIX
    dir = POSIX_IPC_PREFIX;
#else
    dir = "/tmp/";
#endif
  }
  if (dir.back() != '/') {
    dir.push_back('/');
  }
  return dir + name;
}

char* px_ipc_name(char const* name) {
  char const* dir;
  char* dst;
  char const* slash;
  if ((dst = static_cast<char*>(malloc(PATH_MAX))) == nullptr) {
    return nullptr;
  }

  if ((dir = getenv("PX_IPC_NAME")) == nullptr) {
#ifdef POSIX_IPC_PREFIX
    dir = POSIX_IPC_PREFIX;
#else
    dir = "/tmp/";
#endif
  }
  slash = (dir[strlen(dir) - 1] == '/') ? "" : "/";
  snprintf(dst, PATH_MAX, "%s%s%s", dir, slash, name);
  return dst;
}

}  // namespace base
#endif  // BASE_IPC_NAME_H_