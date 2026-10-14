/**
 * @file cxx.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-11-30
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_CXX_H_
#define BASE_CXX_H_
namespace base {

#define CXX_98 199711L
#define CXX_11 201103L
#define CXX_14 201402L
#define CXX_17 201703L
#define CXX_20 202002L
#define CXX_23 202302L

#define SINCE(CXX_STD) __cplusplus >= CXX_STD
#define UNTIL(CXX_STD) __cplusplus < CXX_STD

}  // namespace base
#endif