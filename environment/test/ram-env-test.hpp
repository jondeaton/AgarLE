#pragma once

#include <gtest/gtest.h>
#include <environment/envs/RamEnvironment.hpp>

using namespace agario::env;

extern constexpr bool ram_test_renderable =
#ifdef RENDERABLE
  true
#else
  false
#endif
;