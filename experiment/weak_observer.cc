/**
 * @file weak_observer.cc
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-03
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>
#include <memory>
#include <vector>

class Subject;

class Observer {
 public:
  Observer() = default;

  void SetSubject(std::weak_ptr<Subject> subject) { subject_ = subject; }

  virtual ~Observer() = default;

  void OnNotified() { std::cout << "OnNotified" << std::endl; }

 private:
  std::weak_ptr<Subject> subject_;
};

class Subject {
 public:
  Subject() = default;

  virtual ~Subject() = default;

  void Attatch(std::weak_ptr<Observer> observer) {
    observers_.push_back(observer);
  }

  void Notify() {
    for (auto&& observer : observers_) {
      if (!observer.expired()) {
        auto ptr = observer.lock();
        ptr->OnNotified();
      } else {
        // TODO remove from observers_;
        std::cout << "I know it has been deleted!" << std::endl;
      }
    }
  }

 private:
  std::vector<std::weak_ptr<Observer>> observers_;
};

int main() {
  auto subject = std::make_shared<Subject>();
  {
    auto observer = std::make_shared<Observer>();
    subject->Attatch(std::weak_ptr<Observer>(observer));
  }

  subject->Notify();
  return 0;
}