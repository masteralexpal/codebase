# Portable experiments: C++ standard library only, build on every platform.
add_executable(atomic_exp "atomic_exp.cc")
target_link_libraries(atomic_exp PRIVATE codebase::config third_party::gtest)

add_executable(closure_exp "closure_exp.cc")
target_link_libraries(closure_exp PRIVATE codebase::config third_party::gtest)

add_executable(enable_if_exp "enable_if_exp.cc")
target_link_libraries(enable_if_exp PRIVATE codebase::config)

add_executable(hash_exp "hash_exp.cc")
target_link_libraries(hash_exp PRIVATE codebase::config)

add_executable(reference_counting_exp "reference_counting_exp.cc")
target_link_libraries(reference_counting_exp PRIVATE codebase::config)
enable_asan(reference_counting_exp)

add_executable(store_store_reordering_exp "store_store_reordering_exp.cc")
target_link_libraries(store_store_reordering_exp PRIVATE codebase::config)

add_executable(type_erase_exp "type_erase_exp.cc")
target_link_libraries(type_erase_exp PRIVATE codebase::config)

add_executable(var_exp "var_exp.cc")
target_link_libraries(var_exp PRIVATE codebase::config)

add_executable(weak_observer "weak_observer.cc")
target_link_libraries(weak_observer PRIVATE codebase::config)

# POSIX/Linux-only experiments (pthread, mqueue, sysv ipc, epoll, ...).
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_executable(time_exp "time_exp.cc")
  target_link_libraries(time_exp PRIVATE codebase::config third_party::gtest)

  add_executable(thread_local "thread_local.cc")
  target_link_libraries(thread_local PRIVATE codebase::config third_party::gtest)

  add_executable(pipe_exp "pipe_exp.cc")
  target_link_libraries(pipe_exp PRIVATE codebase::config)

  add_executable(posix_sem_exp "posix_sem_exp.cc")
  target_link_libraries(posix_sem_exp PRIVATE codebase::config)

  add_executable(shared_memory_exp "shared_memory_exp.cc")
  target_link_libraries(shared_memory_exp PRIVATE codebase::config)

  add_executable(store_load_reordering_exp "store_load_reordering_exp.cc")
  target_link_libraries(store_load_reordering_exp PRIVATE codebase::config)

  add_executable(store_load_reordering_preshing "store_load_reordering_preshing.cc")
  target_link_libraries(store_load_reordering_preshing PRIVATE codebase::config)

  add_executable(ls "file_system/ls.cc")
  target_link_libraries(ls PRIVATE codebase::config third_party::gtest)

  add_executable(fcntl_exp "file_system/fcntl_exp.cc")
  target_link_libraries(fcntl_exp PRIVATE codebase::config third_party::gtest)

  add_executable(bio "io/bio.cc")
  target_link_libraries(bio PRIVATE codebase::config third_party::gtest)

  add_executable(pipe "ipc/pipe.cc")
  target_link_libraries(pipe PRIVATE codebase::config)

  add_executable(fifo "ipc/fifo.cc")
  target_link_libraries(fifo PRIVATE codebase::config)

  add_executable(posix_mq "ipc/posix_mq.cc")
  target_link_libraries(posix_mq PRIVATE codebase::config)

  add_executable(unix_socket "ipc/unix_socket.cc")
  target_link_libraries(unix_socket PRIVATE codebase::config)

  add_executable(unix_socket_client "ipc/unix_socket_client.cc")
  target_link_libraries(unix_socket_client PRIVATE codebase::config)

  add_executable(unix_socket_server "ipc/unix_socket_server.cc")
  target_link_libraries(unix_socket_server PRIVATE codebase::config)

  add_executable(unix_socket_multiplex_server "ipc/unix_socket_multiplex_server.cc")
  target_link_libraries(unix_socket_multiplex_server PRIVATE codebase::config)

  add_executable(posix_sem "ipc/posix_sem.cc")
  target_link_libraries(posix_sem PRIVATE codebase::config third_party::gtest)

  add_executable(posix_shm "ipc/posix_shm.cc")
  target_link_libraries(posix_shm PRIVATE codebase::config third_party::gtest)

  add_executable(sysv_mq "ipc/sysv_mq.cc")
  target_link_libraries(sysv_mq PRIVATE codebase::config third_party::gtest)

  add_executable(sysv_mq_client "ipc/sysv_mq_client.cc")
  target_link_libraries(sysv_mq_client PRIVATE codebase::config third_party::gtest)

  add_executable(sysv_mq_server "ipc/sysv_mq_server.cc")
  target_link_libraries(sysv_mq_server PRIVATE codebase::config third_party::gtest)

  add_executable(sysv_sem "ipc/sysv_sem.cc")
  target_link_libraries(sysv_sem PRIVATE codebase::config third_party::gtest)

  add_executable(sysv_shm "ipc/sysv_shm.cc")
  target_link_libraries(sysv_shm PRIVATE codebase::config third_party::gtest)
endif()
