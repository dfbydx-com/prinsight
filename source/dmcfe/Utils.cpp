#include "include/Utils.hpp"

#include <gmp.h>
#include <gmpxx.h>

void Utils::Int64tVecToCfeVec(const std::vector<int64_t>& from, cfe_vec* to) {
  mpz_t temp;
  mpz_init(temp);

  for (size_t i = 0; i < from.size(); i++) {
    mpz_set_si(temp, from[i]);
    cfe_vec_set(to, temp, i);
  }
  mpz_clear(temp);
}
