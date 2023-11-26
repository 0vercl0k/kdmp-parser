// Axel '0vercl0k' Souchet - 2019-2023
#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "kdmp-parser.h"

TEST_CASE("kdmp-parser", "version") {
  CHECK(kdmpparser::Version_t::Major == KDMPPARSER_VERSION_MAJOR);
  CHECK(kdmpparser::Version_t::Minor == KDMPPARSER_VERSION_MINOR);
  CHECK(kdmpparser::Version_t::Patch == KDMPPARSER_VERSION_PATCH);
  CHECK(kdmpparser::Version_t::Release == KDMPPARSER_VERSION_RELEASE);
}
