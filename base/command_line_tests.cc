#include "base/command_line.h"

#include <gtest/gtest.h>
#include <cstdarg>

char **CreateArgv(int count, ...) {
  if (count == 0) {
    return nullptr;
  }
  char **result = new char *[count];
  std::va_list vl;
  va_start(vl, count);
  for (int i = 0; i < count; ++i) {
    result[i] = va_arg(vl, char *);
  }
  va_end(vl);
  return result;
}

void DestroyArgv(char **argv) { delete[] argv; }

TEST(CommandLine, case0) {
  int argc = 5;
  auto result = CreateArgv(argc, "programe_name", "--key0", "--key1=1", "-k=2",
                           "--key3=3");

  base::CommandLine::Initialize(argc, result);
  ASSERT_TRUE(base::CommandLine::Get()->HasKey("key0"));
  ASSERT_FALSE(base::CommandLine::Get()->HasKey("key_1"));
  auto optional = base::CommandLine::Get()->GetValue("key1");
  if (optional.has_value()) {
    ASSERT_EQ(optional.value(), "1");
  }

  ASSERT_FALSE(base::CommandLine::Get()->GetValue("key2").has_value());
  DestroyArgv(result);
}