/**
 * @file helper.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2024-01-04
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <cassert>
#include <cstring>
#include <iostream>

#ifndef EXPERIMENT_HELPER_H_
#define EXPERIMENT_HELPER_H_

#define LOG_ERR_MSG()                                                    \
  std::cerr << "[" << __FILE__ << "(" << __LINE__ << ")@" << __func__    \
            << "()] "                                                    \
            << "Error " << errno << " Occured: " << std::strerror(errno) \
            << std::endl

template <typename T, size_t N>
void Zero(T (&a)[N]) {
  auto* s = memset(a, 0, N * sizeof(T));
  assert(s == a);
}

template <typename T>
void Zero(T* a, size_t n) {
  auto* s = memset(a, 0, n * sizeof(T));
  assert(s == a);
}

#endif  // EXPERIMENT_HELPER_H_