/**
 * @file socket.h
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-09-01
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef NET_SOCKET_H_
#define NET_SOCKET_H_

namespace net {
enum class AddressFamily { kINET4, kINET6, kLOCAL, kUNIX = kLOCAL };

enum class SocketType { kSTREAM, kDGRAM };

enum class SocketState { kINITIAL, kCONNECTED, kCLOSED };

class Socket {
 public:
  Socket(AddressFamily address_family, SocketType socket_type);

  SocketState GetSocketState() const noexcept;

 private:
  bool ChangeState(SocketState const next);

  // native socket handle.
  using SocketHandle = int;
  SocketHandle socket_;
  SocketState socket_state_;
};
}  // namespace net
#endif