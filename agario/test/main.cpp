#include <gtest/gtest.h>
#include <test/test-engine.hpp>
#include <test/test-core.hpp>

namespace { }

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}