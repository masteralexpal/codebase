/**
 * @file reference_counter.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-12-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/memory/reference_counting.h"

#include <cassert>

namespace base {
ReferenceCountingBase::ReferenceCountingBase() noexcept : count_(1) {}

ReferenceCountingBase::ReferenceCountingBase(int initial_count) noexcept
    : count_(initial_count) {}

void ReferenceCountingBase::Add() const noexcept {
  ++count_;
}

bool ReferenceCountingBase::Sub() const noexcept {
  --count_;
  assert(!(count_ < 0));
  return count_ == 0;
}

int ReferenceCountingBase::Count() const noexcept {
  return count_;
}

bool ReferenceCountingBase::Shared() const noexcept {
  return count_ > 1;
}

bool ReferenceCountingBase::Unique() const noexcept {
  return count_ == 1;
}
}  // namespace base