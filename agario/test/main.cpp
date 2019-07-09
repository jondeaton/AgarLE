#include <gtest/gtest.h>
#include <test/test-core.hpp>
#include <test/test-entities.hpp>
#include <test/test-engine.hpp>

namespace { }

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}