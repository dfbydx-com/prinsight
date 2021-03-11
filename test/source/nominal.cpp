#include <doctest/doctest.h>
#include <prinsight/prinsight.h>
#include <prinsight/version.h>

#include <string>

TEST_CASE("Smoke") {
  using namespace prinsight;

  SampleApp app("Tests");
  app.smokeTest();
}

TEST_CASE("Prinsight version") {
  static_assert(std::string_view(PRINSIGHT_VERSION) == std::string_view("0.1.0"));
  CHECK(std::string(PRINSIGHT_VERSION) == std::string("0.1.0"));
}
