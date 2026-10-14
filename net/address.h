/**
 * @file address.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-10-02
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef NET_ADDRESS_H_
#define NET_ADDRESS_H_

#include <netinet/in.h>

#include <string>

namespace net {
class InetAddress {
 public:
  InetAddress(std::string const addess_literal, in_port_t port);

  in_port_t GetPort() const noexcept;

 private:
  union InetAddressInternal {
    sockaddr_in inet4_;
    sockaddr_in6 inet6_;
  };
};
}  // namespace net
#endif