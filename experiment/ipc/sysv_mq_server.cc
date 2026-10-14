/**
 * @file sysv_mq_server.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <thread>

#include "experiment/helper.h"

constexpr int const kProjectionID = 'M';
constexpr char const kMessageQueueName[] = "/tmp/message-queue.sysv";

constexpr long const kMsgType = 0L;

struct Message {
  long type;
  char text[4096];
};

int main() {
  key_t key;
  int mqi;
  int msgflg = IPC_CREAT | IPC_EXCL | 0755;
  Message received_message;
  received_message.type = kMsgType;

  if (open(kMessageQueueName, O_RDWR | O_CREAT | O_TRUNC, 0755) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  if ((key = ftok(kMessageQueueName, kProjectionID)) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  std::cout << "KEY = " << key << std::endl;

  while ((mqi = msgget(key, msgflg)) == -1) {
    if (errno == EEXIST) {
      mqi = msgget(key, 0);
      if (mqi == -1) {
        LOG_ERR_MSG();
        std::exit(EXIT_FAILURE);
      }

      if ((msgctl(mqi, IPC_RMID, nullptr)) == -1) {
        LOG_ERR_MSG();
        std::exit(EXIT_FAILURE);
      }
      std::cout << "Server removed old message queue, id = " << mqi
                << std::endl;
    } else {
      LOG_ERR_MSG();
      std::exit(EXIT_FAILURE);
    }
  }

  std::cout << "Server has created SYSV message queue, id = " << mqi
            << std::endl;

  while (true) {
    if (msgrcv(mqi, received_message.text, sizeof(Message::text), kMsgType,
               0) == -1) {
      LOG_ERR_MSG();
      std::exit(EXIT_FAILURE);
    }

    std::cout << "Server has received: " << received_message.text << std::endl;
  }
}