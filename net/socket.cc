/**
 * @file socket.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-01
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "net/socket.h"
#include <sys/socket.h>

namespace net {

static int ToStandardAddressFamily(AddressFamily address_family) {
  switch (address_family) {
    case net::AddressFamily::kINET4:
      return AF_INET;
    case net::AddressFamily::kINET6:
      return AF_INET6;
    case net::AddressFamily::kLOCAL:
      return AF_LOCAL;
  }
  return -1;
}

static int ToStandardSocketType(SocketType socket_type) {
  switch (socket_type) {
    case net::SocketType::kSTREAM:
      return SOCK_STREAM;
    case net::SocketType::kDGRAM:
      return SOCK_DGRAM;
  }
  return -1;
}

Socket::Socket(AddressFamily address_family, SocketType socket_type) {
  socket_ = socket(ToStandardAddressFamily(address_family),
                   ToStandardSocketType(socket_type), 0);
  if (socket_ == -1) {
  }
}

SocketState Socket::GetSocketState() const noexcept {
  return socket_state_;
}

bool Socket::ChangeState(SocketState const next) {
  switch (socket_state_) {
    case SocketState::kINITIAL: {
      if (next == SocketState::kCONNECTED) {
        socket_state_ = next;
        return true;
      }
      return false;
    }
    case net::SocketState::kCONNECTED: {
      if (next == SocketState::kCLOSED) {
        socket_state_ = next;
        return true;
      }
      return false;
    }
    case net::SocketState::kCLOSED: {
      // Do nothing.
      return false;
    }
  }
  return false;
}
}  // namespace net