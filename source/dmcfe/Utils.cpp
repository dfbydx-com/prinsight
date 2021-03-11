#include "include/Utils.hpp"

#include <gmp.h>
#include <gmpxx.h>

#if defined(ENABLE_PRINSIGHT_TESTING)
#  include <doctest/doctest.h>
#endif

namespace printsight {
  namespace dmcfe {

    void Utils::Int64tVecToCfeVec(const std::vector<int64_t>& from, cfe_vec* to) {
      mpz_t temp;
      mpz_init(temp);

      for (size_t i = 0; i < from.size(); i++) {
        mpz_set_si(temp, from[i]);
        cfe_vec_set(to, temp, i);
      }
      mpz_clear(temp);
    }

#if defined(ENABLE_PRINSIGHT_TESTING)

    TEST_CASE("e2e conversion std vector to cfe vec") {
      const std::vector<int64_t> x1(5, 1);
      cfe_vec x2;

      cfe_vec_init(&x2, x1.size());
      Utils::Int64tVecToCfeVec(x1, &x2);
      cfe_vec_print(&x2);
    }
#endif  // 0

  }  // namespace dmcfe
}  // namespace printsight