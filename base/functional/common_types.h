
#ifndef BASE_FUNCTIONAL_COMMON_TYPES_H_
#define BASE_FUNCTIONAL_COMMON_TYPES_H_

namespace base {

// Specifies the fundamental type of the callable entity.
enum class CallableCategory {
  kFn,       // Regular functions, function pointers, or references.
  kMemFn,    // Pointer to member functions (PMF).
  kFunctor,  // Classes with operator(), including Lambdas.
};

enum class ClosureCategory {
  kOnce,       // You only call once.
  kRepeating,  // You can call infinite times(in theory).
};

}  // namespace base

#endif  //  BASE_FUNCTIONAL_COMMON_TYPES_