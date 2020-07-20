#pragma once

#include <gtest/gtest.h>
#include "inja/inja.hpp"

using json = nlohmann::json;

inline void CHECK(size_t a, size_t b) {
  EXPECT_EQ(a, b);
}

inline void CHECK(const std::string& a, const std::string& b) {
  EXPECT_STREQ(a.c_str(), b.c_str());
}

inline void CHECK(const std::string& a, const char* b) {
  EXPECT_STREQ(a.c_str(), b);
}

#define REQUIRE CHECK
#define CHECK_THROWS_WITH(...)
