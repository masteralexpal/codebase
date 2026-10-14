/**
 * @file sysv_mq_client.cc
 * @author lipingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstdlib>
#include <iostream>

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
  int msgflg = 0755;
  Message send_message;
  send_message.type = kMsgType;
  if ((key = ftok(kMessageQueueName, kProjectionID)) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  std::cout << "KEY = " << key << std::endl;

  if ((mqi = msgget(key, msgflg)) == -1) {
    LOG_ERR_MSG();
    std::exit(EXIT_FAILURE);
  }

  std::cout << "Client has got SYSV message queue, id = " << mqi << std::endl;

  while (true) {
    std::cout << "Please input your message:" << std::endl;
    std::cin >> send_message.text;

    if (msgsnd(mqi, send_message.text, sizeof(Message::text), 0)) {
      LOG_ERR_MSG();
      std::exit(EXIT_FAILURE);
    }

    std::cout << "Message " << send_message.text << ", has been send"
              << std::endl;
  }
}