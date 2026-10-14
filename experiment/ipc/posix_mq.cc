/**
 * @file posix_mq.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-02
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <mqueue.h>
#include <sys/wait.h>
#include <unistd.h>

#include "experiment/helper.h"

// CAUTION! The name of a message queue can have ONLY one splash!
// Ref: https://www.man7.org/linux/man-pages/man7/mq_overview.7.html
static char const message_queue_name[] = "/mqueue.lipingan";
// greater than 10 will cause EINVAL. cat /proc/sys/fs/mqueue/msg_max
static int const kMaxMsg = 10;
static int const kMsgSize = 4096;

int Server(mqd_t message_queue) {
  static char buffer[kMsgSize];
  Zero(buffer);  // not necessary.
  int unsigned priority = 0;

  while (true) {
    int ret = mq_receive(message_queue, buffer, kMsgSize, &priority);
    if (ret == -1) {
      LOG_ERR_MSG();
      return -1;
    }

    buffer[ret] = 0;
    std::cout << "Server received: " << buffer << std::endl;
    std::cout << "Server send back to client" << std::endl;

    ret = mq_send(message_queue, buffer, strlen(buffer), 1u);
    if (ret == -1) {
      LOG_ERR_MSG();
      return -1;
    }
  }
}

int Client(mqd_t message_queue) {
  static char buffer[kMsgSize];
  Zero(buffer);
  int unsigned priority = 0u;
  while (true) {
    std::cout << "Client: Please input your message: " << std::endl;
    std::scanf("%s", buffer);
    std::cout << "Client: send to server" << std::endl;

    int ret = mq_send(message_queue, buffer, strlen(buffer), 1u);
    if (ret == -1) {
      LOG_ERR_MSG();
      return -1;
    }

    ret = mq_receive(message_queue, buffer, kMsgSize, &priority);
    if (ret == -1) {
      LOG_ERR_MSG();
      return -1;
    }
    buffer[ret] = 0;
    std::cout << "Client: received from seriver: " << buffer << std::endl;
  }
}

int main() {
  int oflag = O_RDWR | O_CREAT;
  mode_t mode = 0777;

  struct mq_attr mq_attribute;
  mq_attribute.mq_flags = 0;
  mq_attribute.mq_curmsgs = 0;
  mq_attribute.mq_maxmsg = kMaxMsg;
  mq_attribute.mq_msgsize = kMsgSize;

  mqd_t message_queue = mq_open(message_queue_name, oflag, mode, &mq_attribute);
  if (message_queue == -1) {
    LOG_ERR_MSG();
    std::exit(errno);
  }

  pid_t child_pid;
  if ((child_pid = fork()) == 0) {
    std::exit(Server(message_queue));
  } else {
    int ret = Client(message_queue);
    waitpid(child_pid, nullptr, 0);
    mq_close(message_queue);
    mq_unlink(message_queue_name);
    return ret;
  }
}