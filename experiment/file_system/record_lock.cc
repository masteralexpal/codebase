/**
 * @file record_lock.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

#include "experiment/helper.h"

/**
 * @brief
 *
 */
class RecordLock {
 public:
  RecordLock() {}
  RecordLock(RecordLock const& lock) = delete;
  RecordLock& operator=(RecordLock const& lock) = delete;
  RecordLock(RecordLock&& lock) = delete;
  RecordLock& operator=(RecordLock&& lock) = delete;
  ~RecordLock() {}

  void Acquire() {}

  void Release() {}

  bool TryAcquire() { return false; }
};

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
  char const record_lock_name[] = "/tmp/lipingan.lock";
  int flag = O_RDWR | O_CREAT | O_EXCL;
  mode_t mode = 0755;
  int fd = open(record_lock_name, flag, mode);
  if (fd == -1) {
    LOG_ERR_MSG();
    std::exit(errno);
  }

  struct flock lock;
  memset(&lock, 0, sizeof(lock));

  /* lock entire file */
  lock.l_type = F_RDLCK;    /* F_RDLCK is shared lock */
  lock.l_whence = SEEK_SET; /* offset base is start of the file */
  lock.l_start = 0;         /* starting offset is zero */
  lock.l_len = 0;           /* len is zero, which is a special value
                             representing end of file (no matter
                             how large the file grows in future) */
                            /* F_SETLKW specifies blocking mode */
  if (fcntl(fd, F_SETLKW, &lock) == -1) {
    LOG_ERR_MSG();
    exit(errno);
  }

  /* atomically upgrade shared lock to exclusive lock, but only
   * for bytes in range [10; 15)
   *
   * after this call, the process will hold three lock regions:
   *   [0; 10)        - shared lock
   *   [10; 15)       - exclusive lock
   *   [15; SEEK_END) - shared lock
   */
  lock.l_type = F_WRLCK; /* F_WRLCK is exclusive lock */
  lock.l_whence = SEEK_SET;
  lock.l_start = 10;
  lock.l_len = 5;

  /* F_SETLKW specifies non-blocking mode */
  if (fcntl(fd, F_SETLK, &lock) == -1) {
    perror("fcntl(F_SETLK)");
    exit(EXIT_FAILURE);
  }

  /* release lock for bytes in range [10; 15) */
  lock.l_type = F_UNLCK;

  if (fcntl(fd, F_SETLK, &lock) == -1) {
    perror("fcntl(F_SETLK)");
    exit(EXIT_FAILURE);
  }

  /* close file and release locks for all regions
   * note that locks are released when process calls close() on any
   * descriptor for a lock file */
  close(fd);
}