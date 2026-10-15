/**
 * @file thread.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief Platform-neutral thread logic. The OS-specific pieces (Spawn,
 * PlatformJoin/PlatformDetach/PlatformIsSelf, CurrentThread) live in
 * thread_linux.cc and thread_win.cc.
 * @version 0.1
 * @date 2023-07-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "base/concurrent/thread.h"

#include <atomic>
#include <cstdlib>
#include <exception>
#include <memory>
#include <string>
#include <utility>

namespace base {

Thread::Thread(std::string const& name)
    : handle_{},
      state_(std::make_shared<std::atomic<State>>(kCreated)),
      joinable_(false),
      name_(name) {}

Thread::Thread(Thread&& other) noexcept
    : handle_(other.handle_),
      state_(std::move(other.state_)),
      joinable_(other.joinable_),
      name_(std::move(other.name_)) {
  other.handle_ = {};
  other.joinable_ = false;
}

Thread& Thread::operator=(Thread&& other) noexcept {
  if (this != &other) {
    // Give up the currently owned thread first, with the same policy as the
    // destructor.
    if (joinable_) {
      if (PlatformIsSelf(handle_)) {
        std::abort();
      }
      PlatformJoin(handle_);
    }
    handle_ = other.handle_;
    state_ = std::move(other.state_);
    joinable_ = other.joinable_;
    name_ = std::move(other.name_);
    other.handle_ = {};
    other.joinable_ = false;
  }
  return *this;
}

Thread::~Thread() {
  if (!joinable_) {
    return;
  }
  if (PlatformIsSelf(handle_)) {
    // Joining ourselves would deadlock.
    std::abort();
  }
  // Also reclaims a kFinished thread: its resources are held until the
  // join.
  PlatformJoin(handle_);
}

void Thread::Join() noexcept {
  if (!joinable_) {
    // Never started, already joined/detached, or moved-from.
    std::abort();
  }
  if (PlatformIsSelf(handle_)) {
    // Self-join deadlocks.
    std::abort();
  }
  PlatformJoin(handle_);
  joinable_ = false;
  handle_ = {};
}

void Thread::Detach() noexcept {
  if (!joinable_) {
    // Never started, already joined/detached, or moved-from.
    std::abort();
  }
  // Detaching ourselves is fine on both platforms.
  PlatformDetach(handle_);
  joinable_ = false;
  handle_ = {};
}

std::string Thread::GetName() const noexcept {
  return name_;
}

Thread::State Thread::GetState() const noexcept {
  if (!state_) {
    // Moved-from.
    return kCreated;
  }
  return state_->load(std::memory_order_acquire);
}

// static
void* Thread::StartInternal(void* args) {
  auto* parameters = static_cast<StartRoutineParameters*>(args);

  // Move out everything the routine needs up front: after started.release(),
  // `parameters` belongs to Start() again and must not be touched here.
  std::shared_ptr<std::atomic<State>> const state =
      std::move(parameters->state);
  StartRoutine const start_routine = std::move(parameters->start_routine);

  CurrentThread::SetName(parameters->name);
  parameters->started.release();

  try {
    start_routine();
  } catch (...) {
    // A start routine must not let an exception escape its thread
    // (same contract as std::thread).
    std::terminate();
  }
  state->store(kFinished, std::memory_order_release);
  return nullptr;
}

}  // namespace base
