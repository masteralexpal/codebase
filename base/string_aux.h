/**
 * @file string_auxiliary.h
 * @author Li Pingan (lipingan.dev@outlook.com)
 * @brief
 * @version 0.1
 * @date 2023-08-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef BASE_STRING_AUXILIARY_H_
#define BASE_STRING_AUXILIARY_H_

#include <cstddef>
#include <string>
#include <vector>

namespace base {

/**
 * split a string.
 * @param text the string to be split.
 * @param delimiter delimiter character.
 * @return collection of split substrings.
 */
std::vector<std::string> Split(std::string const& text, char const delimiter);

/**
 * split a string with a given times.
 * @param text the string to be split.
 * @param delimiter delimiter character.
 * @param split_times split times.
 * @return collection of split substrings.
 */
std::vector<std::string> Split(std::string const& text,
                               char delimiter,
                               int split_times);

/**
 * split a string reversely.
 * @param text the string to be split.
 * @param delimiter delimiter character.
 * @return collection of split substrings.
 */
std::vector<std::string> ReverseSplit(std::string const& text, char delimiter);

/**
 * split a string reversely with a given times.
 * @param text the string to be split.
 * @param delimiter delimiter character.
 * @param degree split times.
 * @return collection of split substrings.
 */
std::vector<std::string> ReverseSplit(std::string const& text,
                                      char delimiter,
                                      int degree);

/**
 * trim all whitespaces of left side.
 * @param text
 */
void LeftTrim(std::string& text);

/**
 * trim all whitespaces of right side.
 * @param text
 */
void RightTrim(std::string& text);

/**
 * trim all whitespaces of two sides.
 * @param text
 */
void Trim(std::string& text);

/**
 * trim all whitespaces of left side in copy mode.
 * @param text
 * @return  copied and trimmed string.
 */
std::string LeftTrim(std::string const& text);

/**
 * trim all whitespaces of right side in copy mode.
 * @param text
 * @return copied and trimmed string.
 */
std::string RightTrim(std::string const& text);

/**
 * trim all whitespaces of two sides in copy mode.
 * @param text
 * @return copied and trimmed string.
 */
std::string Trim(std::string const& text);

/**
 * check if a string starts with a given prefix.
 * @param text
 * @param prefix
 * @return copied and trimmed string.
 */
bool StartsWith(std::string const& text, std::string const& prefix);

/**
 * check if a string ends with a given suffix.
 * @param text
 * @param suffix
 * @return
 */
bool EndsWith(std::string const& text, std::string const& suffix);

/**
 *
 * @param bits
 * @param index
 */
void SetBit(size_t& bits, size_t index);

/**
 *
 * @param bits
 * @param index
 */
void ResetBit(size_t& bits, size_t index);

/**
 *
 * @param bits
 * @param index
 */
void FlipBit(size_t& bits, size_t index);
}  // namespace base

#endif