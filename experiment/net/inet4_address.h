/**
 * @file inet4_address.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-10
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef BASE_EXPERIMENT_NET_INET4_ADDRESS_H_
#define BASE_EXPERIMENT_NET_INET4_ADDRESS_H_

#include <netinet/in.h>

#include <string>

namespace net {
class Inet4Address {
 public:
  Inet4Address();

  Inet4Address(std::string const& address, std::uint16_t port);

 private:
  union __packed_address {
    sockaddr_in inet4_;
    sockaddr_in6 inet6_;
  };

  __packed_address address_;
};
}  // namespace net
#endif