#include "include/utils.hpp"

#include <gmpxx.h>

#include <random>

#if defined(ENABLE_PRINSIGHT_TESTING)
#  include <doctest/doctest.h>
#endif

namespace prinsight {

  void Utils::Int64tVecToCfeVec(const std::vector<std::int64_t>& from, cfe_vec* to) {
    mpz_t temp;
    mpz_init(temp);

    for (std::size_t i = 0; i < from.size(); i++) {
      mpz_set_si(temp, from[i]);
      cfe_vec_set(to, temp, i);
    }
    mpz_clear(temp);
  }

  std::string Utils::randomString(std::size_t length) {
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string randomString;

    std::random_device randomDevice;
    std::mt19937 generator(randomDevice());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    for (std::size_t i = 0; i < length; ++i) {
      randomString += CHARACTERS[distribution(generator)];
    }

    return randomString;
  }

  void Utils::zeroize(void* buffer, std::size_t len) {
    if (buffer) {
      memset(buffer, 0x0, len);
    }
  }

#if defined(ENABLE_PRINSIGHT_TESTING)

  TEST_CASE("e2e conversion std vector to cfe vec") {
    const std::vector<std::int64_t> x1(5, 1);
    cfe_vec x2;

    cfe_vec_init(&x2, x1.size());
    Utils::Int64tVecToCfeVec(x1, &x2);
    cfe_vec_print(&x2);
  }
#endif  // 0

}  // namespace prinsight